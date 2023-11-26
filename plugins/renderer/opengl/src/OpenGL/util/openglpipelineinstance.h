#pragma once

#include "Madgine/pipelineloader/pipelineinstance.h"

#include "openglbuffer.h"

#include "../openglpipelineloader.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLPipelineInstance : PipelineInstance {

        OpenGLPipelineInstance(const PipelineConfiguration &config, GLuint pipeline);

        bool bind(VertexFormat format, OpenGLBuffer *instanceBuffer) const;

        virtual WritableByteBuffer mapParameters(size_t index) override;
        virtual WritableByteBuffer mapTempBuffer(size_t space, size_t size, size_t count) const override;

        virtual void bindMesh(RenderTarget *target, const GPUMeshData *mesh, const ByteBuffer &instanceData) const override;
        virtual WritableByteBuffer mapVertices(RenderTarget *target, VertexFormat format, size_t count) const override;
        virtual ByteBufferImpl<uint32_t> mapIndices(RenderTarget *target, size_t count) const override;
        virtual void setGroupSize(size_t groupSize) const override;

        virtual void render(RenderTarget *target) const override;
        virtual void renderRange(RenderTarget *target, size_t elementCount, size_t vertexOffset, IndexType<size_t> indexOffset = {}) const override;
        virtual void renderInstanced(RenderTarget *target, size_t count) const override;

        virtual void bindResources(RenderTarget *target, size_t space, ResourceBlock block) const override;

        mutable GLuint mHandle = 0;
        std::vector<size_t> mConstantBufferSizes;
        bool mDepthChecking;
        mutable GLenum mMode;
        mutable bool mHasIndices = false;
        mutable GLsizei mElementCount;
        mutable size_t mIndexOffset = 0;
    };

    
    struct MADGINE_OPENGL_EXPORT OpenGLPipelineInstanceHandle : OpenGLPipelineInstance {

        OpenGLPipelineInstanceHandle(const PipelineConfiguration &config, OpenGLPipelineLoader::Handle pipeline);

        OpenGLPipelineLoader::Handle mPipeline;
    };

        
    struct MADGINE_OPENGL_EXPORT OpenGLPipelineInstancePtr : OpenGLPipelineInstance {

        OpenGLPipelineInstancePtr(const PipelineConfiguration &config, OpenGLPipelineLoader::Ptr pipeline);

        OpenGLPipelineLoader::Ptr mPipeline;
    };

}
}