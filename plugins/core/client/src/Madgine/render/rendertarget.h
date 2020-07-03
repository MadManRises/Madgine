#pragma once

#include "render/vertex.h"

namespace Engine {
namespace Render {

    struct MADGINE_CLIENT_EXPORT RenderTarget {
        RenderTarget(RenderContext *context);
        RenderTarget(const RenderTarget &) = delete;
        RenderTarget(RenderTarget &&) = default;
        virtual ~RenderTarget();

        void render();
        virtual void setRenderSpace(const Rect2i &space) = 0;
        virtual void renderVertices(Program *program, size_t groupSize, std::vector<Vertex> vertices, std::vector<unsigned short> indices = {}) = 0;
        virtual void renderVertices(Program *program, size_t groupSize, std::vector<Vertex2> vertices, std::vector<unsigned short> indices = {}, TextureHandle texture = 0) = 0;
        virtual void renderMesh(MeshData *mesh, Program *program) = 0;
        virtual void clearDepthBuffer() = 0;		

        virtual const Texture *texture() const = 0;

        virtual bool resize(const Vector2i &size) = 0;
        virtual Vector2i size() const = 0;

        void addRenderPass(RenderPass *pass);
        void removeRenderPass(RenderPass *pass);

        const std::vector<RenderPass*> &renderPasses();

        virtual void beginFrame();
        virtual void endFrame();

    private:
        RenderContext *mContext;

        std::vector<RenderPass*> mRenderPasses;
    };

}
}