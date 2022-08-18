#pragma once

#include "Madgine/pipelineloader/pipelineinstance.h"

#include "openglbuffer.h"

#include "openglssbobuffer.h"

#include "../openglpipelineloader.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLPipelineInstance : PipelineInstance {

        OpenGLPipelineInstance(const PipelineConfiguration &config, OpenGLPipelineLoader::Handle pipeline);

        void bind() const;

        virtual WritableByteBuffer mapParameters(size_t index) override;

        virtual void setInstanceData(const ByteBuffer &data) override;

        virtual void setDynamicParameters(size_t index, const ByteBuffer &data) override;

        virtual void renderMesh(const GPUMeshData *mesh, const Material *material = nullptr) const override;
        virtual void renderMeshInstanced(size_t count, const GPUMeshData *mesh, const Material *material = nullptr) const override;

        virtual void bindTextures(const std::vector<TextureDescriptor> &tex, size_t offset = 0) const override;

        void verify() const;

        mutable GLuint mHandle = 0;
        std::vector<OpenGLBuffer> mUniformBuffers;
#if !OPENGL_ES
        std::vector<OpenGLBuffer> mShaderStorageBuffers;
#else
        std::vector<OpenGLSSBOBuffer> mShaderStorageBuffers;
        mutable OpenGLBuffer mShaderStorageOffsetBuffer = { GL_UNIFORM_BUFFER };
#endif

        OpenGLBuffer mInstanceBuffer = GL_ARRAY_BUFFER;

        OpenGLPipelineLoader::Handle mPipeline;
    };

}
}