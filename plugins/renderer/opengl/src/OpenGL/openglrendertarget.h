#pragma once

#include "Madgine/render/rendertarget.h"
#include "Modules/math/matrix4.h"
#include "util/openglbuffer.h"
#include "util/openglprogram.h"
#include "util/opengltexture.h"

#include "Modules/render/renderpassflags.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLRenderTarget : RenderTarget {

        OpenGLRenderTarget(OpenGLRenderContext *context);
        ~OpenGLRenderTarget();

        virtual void beginFrame() override;

        virtual void setRenderSpace(const Rect2i &space) override;
        virtual void renderVertices(RenderPassFlags flags, size_t groupSize, std::vector<Vertex> vertices, std::vector<unsigned short> indices = {}) override;
        virtual void renderVertices(RenderPassFlags flags, size_t groupSize, std::vector<Vertex2> vertices, std::vector<unsigned short> indices = {}, unsigned int textureId = 0) override;
        //virtual void renderInstancedMesh(RenderPassFlags flags, Resources::MeshData *meshData, const std::vector<Matrix4> &transforms) override;
        virtual void renderMesh(Resources::MeshData *mesh) override;
        virtual void clearDepthBuffer() override;

        //void setupProgram(RenderPassFlags flags = RenderPassFlags_None, unsigned int textureId = 0) override;
    };

}
}