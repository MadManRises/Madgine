#include "../clientlib.h"

#include "rendercontext.h"
#include "rendertarget.h"
#include "renderpass.h"

namespace Engine {
namespace Render {

    RenderTarget::RenderTarget(RenderContext *context)
        : mContext(context)
    {
        mContext->addRenderTarget(this);
    }

    RenderTarget::~RenderTarget()
    {
        mContext->removeRenderTarget(this);
    }

    void RenderTarget::render()
    {
        beginFrame();
        for (RenderPass *pass : mRenderPasses)
            pass->render(this);
        endFrame();
    }

    void RenderTarget::addRenderPass(RenderPass *pass)
    {
        mRenderPasses.insert(
            std::upper_bound(mRenderPasses.begin(), mRenderPasses.end(), pass,
                [](RenderPass *first, RenderPass *second) { return first->priority() < second->priority(); }),
            pass);
    }

    void RenderTarget::removeRenderPass(RenderPass *pass)
    {
        mRenderPasses.erase(std::find(mRenderPasses.begin(), mRenderPasses.end(), pass));
    }

    const std::vector<RenderPass *> &RenderTarget::renderPasses()
    {
        return mRenderPasses;
    }

    void RenderTarget::beginFrame()
    {
    }

    void RenderTarget::endFrame()
    {
    }

}
}