#pragma once

#include "Madgine/render/vertex.h"
#include "Madgine/render/texturedescriptor.h"

namespace Engine {
namespace Render {

    struct MADGINE_CLIENT_EXPORT RenderTarget {
        RenderTarget(RenderContext *context, bool global, std::string name, size_t iterations = 1);
        RenderTarget(const RenderTarget &) = delete;
        RenderTarget(RenderTarget &&) = default;
        virtual ~RenderTarget();

        void render();

        virtual void clearDepthBuffer() = 0;

        virtual TextureDescriptor texture(size_t index = 0, size_t iteration = std::numeric_limits<size_t>::max()) const = 0;
        virtual size_t textureCount() const = 0;
        virtual TextureDescriptor depthTexture() const = 0;

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

        virtual void pushAnnotation(const char *tag) = 0;
        virtual void popAnnotation() = 0;

        virtual void setRenderSpace(const Rect2i &space) = 0;

        RenderContext *context() const;

        size_t iterations() const;

    private:
        RenderContext *mContext;

        std::vector<RenderPass *> mRenderPasses;

        bool mGlobal;

        size_t mFrame;

        std::string mName;

        size_t mIterations;        
    };

}
}