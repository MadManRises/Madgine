#pragma once

#include "Madgine/render/texturedescriptor.h"
#include "Madgine/render/vertex.h"
#include "renderdata.h"

namespace Engine {
namespace Render {

    struct MADGINE_CLIENT_EXPORT RenderTarget : RenderData {
        RenderTarget(RenderContext *context, bool global, std::string name, size_t iterations = 1);
        RenderTarget(const RenderTarget &) = delete;
        RenderTarget(RenderTarget &&) = default;
        virtual ~RenderTarget();

        virtual void clearDepthBuffer() = 0;

        virtual TextureDescriptor texture(size_t index = 0, size_t iteration = std::numeric_limits<size_t>::max()) const;
        virtual size_t textureCount() const;
        virtual TextureDescriptor depthTexture() const;

        virtual Matrix4 getClipSpaceMatrix() const;

        virtual bool resizeImpl(const Vector2i &size) = 0;
        bool resize(const Vector2i &size);
        virtual Vector2i size() const = 0;

        void addRenderPass(RenderPass *pass);
        void removeRenderPass(RenderPass *pass);

        const std::vector<RenderPass *> &renderPasses();

        virtual void beginIteration(size_t iteration) const;
        virtual void endIteration(size_t iteration) const;

        virtual void beginFrame();
        virtual void endFrame();

        virtual void setRenderSpace(const Rect2i &space) = 0;

        size_t iterations() const;

    protected:
        virtual void render() override;

    private:
        std::vector<RenderPass *> mRenderPasses;

        bool mGlobal;

        std::string mName;

        size_t mIterations;
    };

}
}