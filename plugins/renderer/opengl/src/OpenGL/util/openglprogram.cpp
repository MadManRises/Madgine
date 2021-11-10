#include "../opengllib.h"

#include "openglprogram.h"
#include "openglshader.h"

#include "Meta/math/matrix4.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Generic/bytebuffer.h"

#include "openglvertexarray.h"

METATABLE_BEGIN(Engine::Render::OpenGLProgram)
READONLY_PROPERTY(UniformBuffers, uniformBuffers)
METATABLE_END(Engine::Render::OpenGLProgram)

METATABLE_BEGIN(Engine::Render::OpenGLProgram::UniformBufferDescriptor)
MEMBER(mSlot)
MEMBER(mName)
MEMBER(mSize)
METATABLE_END(Engine::Render::OpenGLProgram::UniformBufferDescriptor)

namespace Engine {
namespace Render {

    OpenGLProgram::~OpenGLProgram()
    {
        reset();
    }

    OpenGLProgram::OpenGLProgram(OpenGLProgram &&other)
        : mHandle(std::exchange(other.mHandle, 0))
        , mUniformBuffers(std::move(other.mUniformBuffers))
        , mInstanceBuffer(std::move(other.mInstanceBuffer))
    {
    }

    OpenGLProgram &OpenGLProgram::operator=(OpenGLProgram &&other)
    {
        std::swap(mHandle, other.mHandle);
        std::swap(mUniformBuffers, other.mUniformBuffers);
        std::swap(mShaderStorageBuffers, other.mShaderStorageBuffers);
#if OPENGL_ES
        std::swap(mShaderStorageOffsetBuffer, other.mShaderStorageOffsetBuffer);
#endif
        return *this;
    }

    bool OpenGLProgram::link(typename OpenGLShaderLoader::HandleType vertexShader, typename OpenGLShaderLoader::HandleType pixelShader, typename OpenGLShaderLoader::HandleType geometryShader)
    {
        reset();

        mVertexShader = std::move(vertexShader);
        mPixelShader = std::move(pixelShader);
        mGeometryShader = std::move(geometryShader);

        return true;
    }

    void OpenGLProgram::reset()
    {
        mVertexShader.reset();
        mPixelShader.reset();
        mGeometryShader.reset();

        mUniformBuffers.clear();
        mShaderStorageBuffers.clear();

        if (mHandle) {
            glDeleteProgram(mHandle);
            mHandle = 0;
        }
    }

    void OpenGLProgram::bind(OpenGLVertexArray *format)
    {
        format->bind(this, mInstanceBuffer, mInstanceDataSize);

        if (!mHandle) {
            if (mVertexShader->mType != VertexShader || (mPixelShader && mPixelShader->mType != PixelShader) || (mGeometryShader && mGeometryShader->mType != GeometryShader))
                std::terminate();

            mHandle = glCreateProgram();
            glAttachShader(mHandle, mVertexShader->mHandle);
            if (mPixelShader)
                glAttachShader(mHandle, mPixelShader->mHandle);
            if (mGeometryShader)
                glAttachShader(mHandle, mGeometryShader->mHandle);

            glLinkProgram(mHandle);
            // check for linking errors
            GLint success;
            char infoLog[512];
            glGetProgramiv(mHandle, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(mHandle, 512, NULL, infoLog);
                LOG_ERROR("ERROR::SHADER::PROGRAM::LINKING_FAILED");
                LOG_ERROR(infoLog);
                std::terminate();
            }

#if OPENGL_ES
            GLuint perApplicationIndex = glGetUniformBlockIndex(mHandle, "PerApplication");
            if (perApplicationIndex != GL_INVALID_INDEX) {
                glUniformBlockBinding(mHandle, perApplicationIndex, 0);
                GL_CHECK();
            }

            GLuint perFrameIndex = glGetUniformBlockIndex(mHandle, "PerFrame");
            if (perFrameIndex != GL_INVALID_INDEX) {
                glUniformBlockBinding(mHandle, perFrameIndex, 1);
                GL_CHECK();
            }

            GLuint perObjectIndex = glGetUniformBlockIndex(mHandle, "PerObject");
            if (perObjectIndex != GL_INVALID_INDEX) {
                glUniformBlockBinding(mHandle, perObjectIndex, 2);
                GL_CHECK();
            }

            GLuint SSBOIndex = glGetUniformBlockIndex(mHandle, "SSBO");
            if (SSBOIndex != GL_INVALID_INDEX) {
                glUniformBlockBinding(mHandle, SSBOIndex, 3);
                GL_CHECK();
            }

            GLuint SSBOOffsetsIndex = glGetUniformBlockIndex(mHandle, "SSBO_Info");
            if (SSBOOffsetsIndex != GL_INVALID_INDEX) {
                glUniformBlockBinding(mHandle, SSBOOffsetsIndex, 4);
                GL_CHECK();
            }

#endif

        }
        glUseProgram(mHandle);
        GL_CHECK();

        for (size_t i = 0; i < mUniformBuffers.size(); ++i) {
            glBindBufferBase(GL_UNIFORM_BUFFER, i, mUniformBuffers[i].handle());
            GL_CHECK();
        }
#if !OPENGL_ES
        for (size_t i = 0; i < mShaderStorageBuffers.size(); ++i) {
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, i, mShaderStorageBuffers[i].handle());
            GL_CHECK();
        }
#else
        size_t size = mShaderStorageBuffers.size();
        if (size > 0) {
            size = (((size - 1) / 4) + 1) * 4;
            mShaderStorageOffsetBuffer.resize(size * sizeof(unsigned int));
            {
                auto offsets = mShaderStorageOffsetBuffer.mapData().cast<unsigned int[]>();
                size_t i = 0;
                for (const OpenGLSSBOBuffer &buffer : mShaderStorageBuffers) {
                    offsets[i++] = buffer.offset();
                }
            }
            glBindBufferBase(GL_UNIFORM_BUFFER, 4, mShaderStorageOffsetBuffer.handle());
            GL_CHECK();
        }
#endif
    }

    void OpenGLProgram::unbind(OpenGLVertexArray* format) {
        uint8_t index = format->mFormat;
        format->unbind(this);
    }

    void OpenGLProgram::setParametersSize(size_t index, size_t size)
    {
        if (mUniformBuffers.size() <= index) {
            mUniformBuffers.reserve(index + 1);
            while (mUniformBuffers.size() <= index)
                mUniformBuffers.emplace_back(GL_UNIFORM_BUFFER);
        }
        mUniformBuffers[index].resize(size);
    }

    WritableByteBuffer OpenGLProgram::mapParameters(size_t index)
    {
        return mUniformBuffers[index].mapData();
    }

    void OpenGLProgram::setInstanceDataSize(size_t size)
    {
        mInstanceDataSize = size;
    }

    void OpenGLProgram::setInstanceData(const ByteBuffer &data)
    {
        mInstanceBuffer.setData(data);
    }

    void OpenGLProgram::setDynamicParameters(size_t index, const ByteBuffer &data)
    {
        if (mShaderStorageBuffers.size() <= index) {
            mShaderStorageBuffers.reserve(index + 1);
            while (mShaderStorageBuffers.size() <= index)
                mShaderStorageBuffers.emplace_back(
#if OPENGL_ES
                    GL_UNIFORM_BUFFER
#else
                    GL_SHADER_STORAGE_BUFFER
#endif
                );
        }

        mShaderStorageBuffers[index].resize(data.mSize);

        if (data.mSize > 0) {
            auto target = mShaderStorageBuffers[index].mapData();
            std::memcpy(target.mData, data.mData, data.mSize);
        }
    }

    std::vector<OpenGLProgram::UniformBufferDescriptor> OpenGLProgram::uniformBuffers()
    {
        int maxLength;
        glGetProgramiv(mHandle, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &maxLength);
        GL_CHECK();
        std::unique_ptr<char[]> name = std::make_unique<char[]>(maxLength + 1);

        int uniformCount;
        glGetProgramiv(mHandle, GL_ACTIVE_UNIFORM_BLOCKS, &uniformCount);
        GL_CHECK();

        std::vector<UniformBufferDescriptor> result;

        for (int i = 0; i < uniformCount; ++i) {
            int size;
            glGetActiveUniformBlockiv(mHandle, i, GL_UNIFORM_BLOCK_DATA_SIZE, &size);

            glGetActiveUniformBlockName(mHandle, i, maxLength, NULL, name.get());
            GL_CHECK();

            int index;
            glGetActiveUniformBlockiv(mHandle, i, GL_UNIFORM_BLOCK_BINDING, &index);
            GL_CHECK();

            result.push_back({ name.get(), static_cast<uint32_t>(index), static_cast<uint32_t>(size) });
        }

        return result;
    }

    void OpenGLProgram::verify()
    {
#if !OPENGL_ES
        int maxLength;
        std::unique_ptr<char[]> name;

        int uniformCount;
        glGetProgramiv(mHandle, GL_ACTIVE_UNIFORM_BLOCKS, &uniformCount);
        GL_CHECK();

        for (int i = 0; i < uniformCount; ++i) {
            int expectedSize;
            glGetActiveUniformBlockiv(mHandle, i, GL_UNIFORM_BLOCK_DATA_SIZE, &expectedSize);
            GL_CHECK();

            int index;
            glGetActiveUniformBlockiv(mHandle, i, GL_UNIFORM_BLOCK_BINDING, &index);
            GL_CHECK();

            int bufferName;
            glGetIntegeri_v(GL_UNIFORM_BUFFER_BINDING, index, &bufferName);
            GL_CHECK();

            int size;
            glBindBuffer(GL_UNIFORM_BUFFER, bufferName);
            glGetBufferParameteriv(GL_UNIFORM_BUFFER, GL_BUFFER_SIZE, &size);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
            GL_CHECK();

            if (size < expectedSize) {
                if (!name) {
                    glGetProgramiv(mHandle, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &maxLength);
                    GL_CHECK();
                    name = std::make_unique<char[]>(maxLength + 1);
                }
                glGetActiveUniformBlockName(mHandle, i, maxLength, NULL, name.get());
                GL_CHECK();

                LOG_ERROR("Uniform Buffer '" << name.get() << "'(index: " << index << ", size: " << expectedSize << ") is bound with too small buffer " << bufferName << " of size " << size);
            } /*else {
                if (!name) {
                    glGetProgramiv(mHandle, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &maxLength);
                    GL_CHECK();
                    name = std::make_unique<char[]>(maxLength + 1);
                }
                glGetActiveUniformBlockName(mHandle, i, maxLength, NULL, name.get());
                GL_CHECK();

				LOG("Uniform Buffer '" << name.get() << "' " << index << ", " << size << "/" << expectedSize);
            }*/
        }
#endif
    }

}
}