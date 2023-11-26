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
        , mConstantBufferSizes(config.bufferSizes)
        , mDepthChecking(config.depthChecking)
    {
    }

    bool OpenGLPipelineInstance::bind(VertexFormat format, OpenGLBuffer *instanceBuffer) const
    {
        glUseProgram(mHandle);
        GL_CHECK();

        if (mDepthChecking)
            glEnable(GL_DEPTH_TEST);
        else
            glDisable(GL_DEPTH_TEST);

        OpenGLRenderContext::getSingleton().bindFormat(format, instanceBuffer, mInstanceDataSize);

        return true;
    }

    WritableByteBuffer OpenGLPipelineInstance::mapParameters(size_t index)
    {
        static size_t alignment = []() {
            GLint alignment;
            glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);
            GL_CHECK();
            return alignment;
        }();
        Block block = OpenGLRenderContext::getSingleton().mTempAllocator.allocate(mConstantBufferSizes[index], alignment);
        auto [buffer, offset] = OpenGLRenderContext::getSingleton().mTempMemoryHeap.resolve(block.mAddress);

        glBindBufferRange(GL_UNIFORM_BUFFER, index, buffer, offset, block.mSize);
        GL_CHECK();

        return { block.mAddress, block.mSize };
    }

    void OpenGLPipelineInstance::render(RenderTarget *target) const
    {
        if (mHasIndices) {
            glDrawElements(mMode, mElementCount, GL_UNSIGNED_INT, reinterpret_cast<const void *>(mIndexOffset));
        } else
            glDrawArrays(mMode, 0, mElementCount);
        GL_CHECK();

        static_cast<OpenGLRenderTarget *>(target)->context()->unbindFormat();

        mHasIndices = false;
    }

    void OpenGLPipelineInstance::renderRange(RenderTarget *target, size_t elementCount, size_t vertexOffset, IndexType<size_t> indexOffset) const
    {
        assert(elementCount <= mElementCount);

        if (mHasIndices) {
            assert(indexOffset);
            glDrawElementsBaseVertex(mMode, elementCount, GL_UNSIGNED_INT, reinterpret_cast<const void *>(mIndexOffset + indexOffset * sizeof(uint32_t)), vertexOffset);
        } else
            glDrawArrays(mMode, vertexOffset, elementCount);
        GL_CHECK();
    }

    void OpenGLPipelineInstance::renderInstanced(RenderTarget *target, size_t count) const
    {
        if (mHasIndices) {
            glDrawElementsInstanced(mMode, mElementCount, GL_UNSIGNED_INT, reinterpret_cast<const void *>(mIndexOffset), count);
        } else
            glDrawArraysInstanced(mMode, 0, mElementCount, count);
        GL_CHECK();

        static_cast<OpenGLRenderTarget *>(target)->context()->unbindFormat();

        mHasIndices = false;
    }

    WritableByteBuffer OpenGLPipelineInstance::mapTempBuffer(size_t space, size_t size, size_t count) const
    {
        Block block = OpenGLRenderContext::getSingleton().mTempAllocator.allocate(size * count, 16);
        auto [buffer, offset] = OpenGLRenderContext::getSingleton().mTempMemoryHeap.resolve(block.mAddress);

        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 4 + (space - 1), buffer, offset, block.mSize);
        GL_CHECK();

        return { block.mAddress, block.mSize };
    }

    void OpenGLPipelineInstance::bindMesh(RenderTarget *target, const GPUMeshData *m, const ByteBuffer &instanceData) const
    {
        assert(mInstanceDataSize % 16 == 0);

        const OpenGLMeshData *mesh = static_cast<const OpenGLMeshData *>(m);

        OpenGLBuffer instanceBuffer { GL_ARRAY_BUFFER };
        if (instanceData.mSize > 0) {
            instanceBuffer.setData(instanceData);
        }

        if (!bind(mesh->mFormat, instanceData.mSize > 0 ? &instanceBuffer : nullptr))
            return;

        mesh->mVertices.bindVertex(mesh->mVertexSize);

        mMode = sModes[mesh->mGroupSize - 1];

        if (mesh->mIndices) {
            mesh->mIndices.bind();
            mHasIndices = true;
        } else {
            mHasIndices = false;
        }

        mElementCount = mesh->mElementCount;

        mIndexOffset = 0;
    }

    WritableByteBuffer OpenGLPipelineInstance::mapVertices(RenderTarget *target, VertexFormat format, size_t count) const
    {
        if (!bind(format, nullptr))
            return {};

        Block block = OpenGLRenderContext::getSingleton().mTempAllocator.allocate(format.stride() * count);
        auto [buffer, offset] = OpenGLRenderContext::getSingleton().mTempMemoryHeap.resolve(block.mAddress);

        glBindVertexBuffer(0, buffer, offset, format.stride());
        GL_CHECK();

        mElementCount = count;

        return { block.mAddress, block.mSize };
    }

    ByteBufferImpl<uint32_t> OpenGLPipelineInstance::mapIndices(RenderTarget *target, size_t count) const
    {
        Block block = OpenGLRenderContext::getSingleton().mTempAllocator.allocate(sizeof(uint32_t) * count);
        auto [buffer, offset] = OpenGLRenderContext::getSingleton().mTempMemoryHeap.resolve(block.mAddress);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
        GL_CHECK();

        mIndexOffset = offset;

        mElementCount = count;
        mHasIndices = true;

        return { static_cast<uint32_t *>(block.mAddress), block.mSize };
    }

    void OpenGLPipelineInstance::setGroupSize(size_t groupSize) const
    {
        mMode = sModes[groupSize - 1];
    }

    void OpenGLPipelineInstance::bindResources(RenderTarget *target, size_t space, ResourceBlock block) const
    {
        if (block) {
            OpenGLResourceBlock<> *textures = block;
            for (size_t i = 0; i < textures->mSize; ++i) {
                glActiveTexture(GL_TEXTURE0 + 4 * (space - 1) + i);
                glBindTexture(textures->mResources[i].mTarget, textures->mResources[i].mHandle);
                GL_CHECK();
            }
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