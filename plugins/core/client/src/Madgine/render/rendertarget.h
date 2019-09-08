#pragma once

#include "Modules/math/vector2.h"
#include "renderpass.h"
#include "vertex.h"

namespace Engine {
namespace Render {

	typedef int RenderPassFlags;
    enum RenderPassFlags_ {
        RenderPassFlags_None = 0,
		RenderPassFlags_DataFormat2 = 1 << 0,
        RenderPassFlags_NoLighting = 1 << 1,
		RenderPassFlags_NoPerspective = 1 << 2
	};

	/*enum RenderPassFlags_ {
        RenderPassFlags_None = 0,
        RenderPassFlags_HasColor = 1 << 0,
        RenderPassFlags_HasUV = 1 << 1,
        RenderPassFlags_HasPos2 = 1 << 2,
        RenderPassFlags_HasNormal = 1 << 3,
        RenderPassFlags_NoLighting = 1 << 4,
        RenderPassFlags_NoPerspective = 1 << 5,
        RenderPassFlags_DistanceField = 1 << 6
    };*/

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