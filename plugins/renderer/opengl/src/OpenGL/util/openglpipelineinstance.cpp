#include "../opengllib.h"

#include "openglpipelineinstance.h"
#include "openglshader.h"

#include "Meta/math/matrix4.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Generic/bytebuffer.h"

#include "openglvertexarray.h"

namespace Engine {
namespace Render {

    OpenGLPipelineInstance::OpenGLPipelineInstance(const PipelineConfiguration &config, OpenGLPipelineLoader::Handle pipeline)
        : PipelineInstance(config)
        , mPipeline(std::move(pipeline))
    {
        mUniformBuffers.reserve(config.bufferSizes.size());
        for (size_t i = 0; i < config.bufferSizes.size(); ++i) {
            mUniformBuffers.emplace_back(GL_UNIFORM_BUFFER, ByteBuffer{ nullptr, config.bufferSizes[i] });
        }
    }

    void OpenGLPipelineInstance::bind() const
    {
        if (mInstanceDataSize > 0) {
            assert(mInstanceDataSize % 16 == 0);

            mInstanceBuffer.bind();
            for (size_t i = 0; i < mInstanceDataSize / 16; ++i) {
                glVertexAttribPointer(7 + i, 4, GL_FLOAT, GL_FALSE, mInstanceDataSize, reinterpret_cast<void *>(i * sizeof(float[4])));
                GL_CHECK();
                glVertexAttribDivisor(7 + i, 1);
                GL_CHECK();
                glEnableVertexAttribArray(7 + i);
                GL_CHECK();
            }
        } else {
            glDisableVertexAttribArray(7);
            glDisableVertexAttribArray(8);
        }

        if (!mHandle) {
            if (mPipeline.available())
                mHandle = mPipeline->handle();
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

    WritableByteBuffer OpenGLPipelineInstance::mapParameters(size_t index)
    {
        return mUniformBuffers[index].mapData();
    }

    void OpenGLPipelineInstance::setInstanceData(const ByteBuffer &data)
    {
        mInstanceBuffer.setData(data);
    }

    void OpenGLPipelineInstance::setDynamicParameters(size_t index, const ByteBuffer &data)
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

    void OpenGLPipelineInstance::verify() const
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