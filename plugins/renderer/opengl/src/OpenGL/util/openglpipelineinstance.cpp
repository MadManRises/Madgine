#include "../opengllib.h"

#include "openglpipelineinstance.h"
#include "openglshader.h"

#include "Meta/math/matrix4.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Generic/bytebuffer.h"

#include "openglvertexarray.h"

#include "../openglmeshdata.h"

#include "../openglrendercontext.h"
#include "../openglrendertarget.h"

namespace Engine {
namespace Render {

    static constexpr GLenum sModes[] {
        GL_POINTS,
        GL_LINES,
        GL_TRIANGLES
    };

    OpenGLPipelineInstance::OpenGLPipelineInstance(const PipelineConfiguration &config, GLuint pipeline)
        : PipelineInstance(config)
        , mHandle(pipeline)
    {
        mUniformBuffers.reserve(config.bufferSizes.size());
        for (size_t i = 0; i < config.bufferSizes.size(); ++i) {
            mUniformBuffers.emplace_back(GL_UNIFORM_BUFFER, ByteBuffer { nullptr, config.bufferSizes[i] });
        }
    }

    bool OpenGLPipelineInstance::bind(VertexFormat format, OpenGLBuffer *instanceBuffer) const
    {
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
                auto offsets = mShaderStorageOffsetBuffer.mapData<unsigned int[]>();
                size_t i = 0;
                for (const OpenGLSSBOBuffer &buffer : mShaderStorageBuffers) {
                    offsets[i++] = buffer.offset();
                }
            }
            glBindBufferBase(GL_UNIFORM_BUFFER, 4, mShaderStorageOffsetBuffer.handle());
            GL_CHECK();
        }
#endif

        OpenGLRenderContext::getSingleton().bindFormat(format, instanceBuffer, mInstanceDataSize);

        return true;
    }

    WritableByteBuffer OpenGLPipelineInstance::mapParameters(size_t index)
    {
        return mUniformBuffers[index].mapData();
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

    void OpenGLPipelineInstance::renderMesh(RenderTarget *target, const GPUMeshData *m) const
    {
        const OpenGLMeshData *mesh = static_cast<const OpenGLMeshData *>(m);

        mesh->mVertices.bindVertex(mesh->mVertexSize);

        if (!bind(mesh->mFormat, nullptr))
            return;

        GLenum mode = sModes[mesh->mGroupSize - 1];

        if (mesh->mIndices) {
            mesh->mIndices.bind();
            glDrawElements(mode, mesh->mElementCount, GL_UNSIGNED_INT, 0);
        } else
            glDrawArrays(mode, 0, mesh->mElementCount);
        GL_CHECK();

        static_cast<OpenGLRenderTarget *>(target)->context()->unbindFormat();
    }

    void OpenGLPipelineInstance::renderMeshInstanced(RenderTarget *target, size_t count, const GPUMeshData *m, const ByteBuffer &instanceData) const
    {
        assert(mInstanceDataSize * count == instanceData.mSize);
        assert(mInstanceDataSize % 16 == 0);

        const OpenGLMeshData *mesh = static_cast<const OpenGLMeshData *>(m);

        OpenGLBuffer instanceBuffer { GL_ARRAY_BUFFER, instanceData };

        mesh->mVertices.bindVertex(mesh->mVertexSize);

        if (!bind(mesh->mFormat, &instanceBuffer))
            return;

        GLenum mode = sModes[mesh->mGroupSize - 1];

        if (mesh->mIndices) {
            mesh->mIndices.bind();
            glDrawElementsInstanced(mode, mesh->mElementCount, GL_UNSIGNED_INT, 0, count);
        } else
            glDrawArraysInstanced(mode, 0, mesh->mElementCount, count);
        GL_CHECK();

        static_cast<OpenGLRenderTarget *>(target)->context()->unbindFormat();
    }

    void OpenGLPipelineInstance::bindTextures(RenderTarget *target, const std::vector<TextureDescriptor> &tex, size_t offset) const
    {
        for (size_t i = 0; i < tex.size(); ++i) {
            glActiveTexture(GL_TEXTURE0 + offset + i);
            GLenum type;
            switch (tex[i].mType) {
            case TextureType_2D:
                type = GL_TEXTURE_2D;
                break;
#if MULTISAMPLING
            case TextureType_2DMultiSample:
                type = GL_TEXTURE_2D_MULTISAMPLE;
                break;
#endif
            case TextureType_Cube:
                type = GL_TEXTURE_CUBE_MAP;
                break;
            default:
                throw 0;
            }
            glBindTexture(type, tex[i].mTextureHandle);
            GL_CHECK();
        }
    }

    OpenGLPipelineInstanceHandle::OpenGLPipelineInstanceHandle(const PipelineConfiguration &config, OpenGLPipelineLoader::Handle pipeline)
        : OpenGLPipelineInstance(config, pipeline->handle())
        , mPipeline(std::move(pipeline))
    {
    }

    
    OpenGLPipelineInstancePtr::OpenGLPipelineInstancePtr(const PipelineConfiguration &config, OpenGLPipelineLoader::Ptr pipeline)
        : OpenGLPipelineInstance(config, pipeline->handle())
        , mPipeline(std::move(pipeline))
    {
    }

}
}