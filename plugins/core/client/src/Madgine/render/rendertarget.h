#pragma once

#include "Modules/math/vector2.h"
#include "renderpass.h"
#include "vertex.h"

namespace Engine {
namespace Render {

    struct MADGINE_CLIENT_EXPORT RenderTarget {
        RenderTarget(RenderWindow *window, Scene::Camera *camera, const Vector2 &size);
        RenderTarget(const RenderTarget &) = delete;
        RenderTarget(RenderTarget &&) = default;
        virtual ~RenderTarget();

        Scene::Camera *camera() const;

        virtual void render() = 0;
        virtual void renderVertices(RenderPassFlags flags, size_t groupSize, Vertex *vertices, size_t vertexCount, unsigned int *indices = nullptr, size_t indexCount = 0) = 0;
        virtual void renderVertices(RenderPassFlags flags, size_t groupSize, Vertex2 *vertices, size_t vertexCount, unsigned int *indices = nullptr, size_t indexCount = 0, unsigned int textureId = 0) = 0;
        virtual void renderInstancedMesh(RenderPassFlags flags, void *meshData, const std::vector<Matrix4> &transforms) = 0;
        virtual void clearDepthBuffer() = 0;

        virtual uint32_t textureId() const = 0;

        virtual bool resize(const Vector2 &size);

        const Vector2 &getSize();

        void addPreRenderPass(std::unique_ptr<RenderPass> &&pass);
        void addPostRenderPass(std::unique_ptr<RenderPass> &&pass);

        const std::vector<std::unique_ptr<RenderPass>> &preRenderPasses();
        const std::vector<std::unique_ptr<RenderPass>> &postRenderPasses();

    private:
        RenderWindow *mWindow;
        Scene::Camera *mCamera;
        Vector2 mSize;

        std::vector<std::unique_ptr<RenderPass>> mPreRenderPasses;
        std::vector<std::unique_ptr<RenderPass>> mPostRenderPasses;
    };

}
}