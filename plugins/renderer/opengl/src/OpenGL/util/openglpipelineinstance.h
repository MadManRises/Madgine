#pragma once

#include "Madgine/pipelineloader/pipelineinstance.h"

#include "openglbuffer.h"

#include "openglssbobuffer.h"

#include "../openglpipelineloader.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLPipelineInstance : PipelineInstance {

        OpenGLPipelineInstance(const PipelineConfiguration &config, GLuint pipeline);

        bool bind(VertexFormat format, OpenGLBuffer *instanceBuffer) const;

        virtual WritableByteBuffer mapParameters(size_t index) override;

        virtual void renderMesh(RenderTarget *target, const GPUMeshData *mesh) const override;
        virtual void renderMeshInstanced(RenderTarget *target, size_t count, const GPUMeshData *mesh, const ByteBuffer &instanceData) const override;

        virtual void bindTextures(RenderTarget *target, const std::vector<TextureDescriptor> &tex, size_t offset = 0) const override;

        mutable GLuint mHandle = 0;
        std::vector<OpenGLBuffer> mUniformBuffers;
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