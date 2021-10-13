#pragma once

#include "Madgine/render/rendertarget.h"
#include "gpumeshdata.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLRenderTarget : RenderTarget {

        OpenGLRenderTarget(OpenGLRenderContext *context, bool global, std::string name, size_t iterations = 1);
        ~OpenGLRenderTarget();

        virtual void beginIteration(size_t iteration) override;

        virtual void pushAnnotation(const char *tag) override;
        virtual void popAnnotation() override;

        virtual void setRenderSpace(const Rect2i &space) override;
        virtual void renderVertices(Program *program, size_t groupSize, std::vector<Vertex> vertices, std::vector<unsigned short> indices = {}) override;
        virtual void renderVertices(Program *program, size_t groupSize, std::vector<Vertex2> vertices, std::vector<unsigned short> indices = {}, const GPUMeshData::Material *material = nullptr) override;
        virtual void renderMesh(GPUMeshData *mesh, Program *program, const GPUMeshData::Material *material = nullptr) override;
        virtual void renderMeshInstanced(size_t count, GPUMeshData *mesh, Program *program, const GPUMeshData::Material *material = nullptr) override;
        virtual void clearDepthBuffer() override;

        virtual void bindTextures(const std::vector<TextureDescriptor> &tex, size_t offset = 0) override;

        //void setupProgram(RenderPassFlags flags = RenderPassFlags_None, unsigned int textureId = 0) override;
    };

}
}