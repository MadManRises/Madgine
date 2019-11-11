#include "../clientlib.h"

#include "rendertarget.h"
#include "rendercontext.h"

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
        mRenderPasses.push_back(pass);
    }

    void RenderTarget::removeRenderPass(RenderPass *pass)
    {
        mRenderPasses.erase(std::find(mRenderPasses.begin(), mRenderPasses.end(), pass));
    }

    const std::vector<RenderPass*> &RenderTarget::renderPasses()
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