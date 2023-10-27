#pragma once

#include "Madgine/render/texturedescriptor.h"
#include "Madgine/render/vertex.h"
#include "renderdata.h"

namespace Engine {
namespace Render {

    struct MADGINE_CLIENT_EXPORT RenderTarget : RenderData {
        RenderTarget(RenderContext *context, bool global, std::string name, bool flipFlop = false, RenderTarget * blitSource = nullptr);
        RenderTarget(const RenderTarget &) = delete;
        RenderTarget(RenderTarget &&) = default;
        virtual ~RenderTarget();

        virtual void clearDepthBuffer() = 0;

        virtual TextureDescriptor texture(size_t index = 0) const;
        virtual size_t textureCount() const;
        virtual TextureDescriptor depthTexture() const;

        virtual Matrix4 getClipSpaceMatrix() const;

        virtual bool resizeImpl(const Vector2i &size) = 0;
        bool resize(const Vector2i &size);
        virtual Vector2i size() const = 0;

        void addRenderPass(RenderPass *pass);
        void removeRenderPass(RenderPass *pass);

        const std::vector<RenderPass *> &renderPasses();
        std::vector<const RenderPass *> renderPasses() const;

        virtual void beginIteration(bool flipFlopping, size_t targetIndex, size_t targetCount, size_t targetSubresourceIndex) const;
        virtual void endIteration() const;

        virtual bool skipFrame();

        virtual void beginFrame();
        virtual void endFrame();

        virtual void pushAnnotation(const char *tag) = 0;
        virtual void popAnnotation() = 0;

        virtual void setRenderSpace(const Rect2i &space) = 0;

        bool canFlipFlop() const;

        const std::string &name() const;

        RenderContext *context() const;

        const RenderTarget *blitSource() const;

    protected:
        virtual Threading::ImmediateTask<void> render(RenderContext *context) override;

        RenderTarget *mBlitSource;

        RenderContext *mContext;

        size_t mFlipFlopIndices[4] = { 0 };

    private:
        std::vector<RenderPass *> mRenderPasses;

        bool mGlobal;

        std::string mName;

        bool mFlipFlop;
    };

}
}