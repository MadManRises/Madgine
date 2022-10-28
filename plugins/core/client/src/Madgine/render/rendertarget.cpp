#include "../clientlib.h"

#include "rendercontext.h"
#include "renderpass.h"
#include "rendertarget.h"

#include "Meta/math/matrix4.h"

namespace Engine {
namespace Render {

    RenderTarget::RenderTarget(RenderContext *context, bool global, std::string name, size_t iterations)
        : mContext(context)
        , mGlobal(global)
        , mName(std::move(name))
        , mIterations(iterations)
    {
        if (global)
            mContext->addRenderTarget(this);
    }

    RenderTarget::~RenderTarget()
    {
        for (RenderPass *pass : mRenderPasses) {
            pass->shutdown();
        }
        if (mGlobal)
            mContext->removeRenderTarget(this);
    }

    void RenderTarget::render()
    {
        if (mContext->frame() == mFrame)
            return;
        mFrame = mContext->frame();

        beginFrame();

        for (RenderPass *pass : mRenderPasses)
            pass->preRender();
        for (size_t iteration = 0; iteration < mIterations; ++iteration) {
            beginIteration(iteration);
            for (RenderPass *pass : mRenderPasses)
                pass->render(this, iteration);
            endIteration(iteration);
        }

        endFrame();
    }

    void RenderTarget::addRenderPass(RenderPass *pass)
    {
        mRenderPasses.insert(
            std::upper_bound(mRenderPasses.begin(), mRenderPasses.end(), pass,
                [](RenderPass *first, RenderPass *second) { return first->priority() < second->priority(); }),
            pass);
        pass->setup(this);
    }

    void RenderTarget::removeRenderPass(RenderPass *pass)
    {
        pass->shutdown();
        std::erase(mRenderPasses, pass);
    }

    const std::vector<RenderPass *> &RenderTarget::renderPasses()
    {
        return mRenderPasses;
    }

    void RenderTarget::beginFrame()
    {
        if (!mName.empty())
            pushAnnotation(mName.c_str());
    }

    void RenderTarget::endFrame()
    {
        if (!mName.empty())
            popAnnotation();
    }

    void RenderTarget::beginIteration(size_t iteration) const
    {
    }

    void RenderTarget::endIteration(size_t iteration) const
    {
    }

    size_t RenderTarget::iterations() const
    {
        return mIterations;
    }

    RenderContext *RenderTarget::context() const
    {
        return mContext;
    }

    bool RenderTarget::resize(const Vector2i &size)
    {
        bool resized = resizeImpl(size);
        if (resized) {
            for (RenderPass *pass : mRenderPasses)
                pass->onTargetResize(size);
        }
        return resized;
    }

    Matrix4 RenderTarget::getClipSpaceMatrix() const
    {
        return Matrix4::IDENTITY;
    }

}
}