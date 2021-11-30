#include "../clientlib.h"

#include "rendercontext.h"

#include "Generic/container/reverseIt.h"

#include "Generic/container/safeiterator.h"

#include "Modules/threading/workgroupstorage.h"

#include "rendertarget.h"

namespace Engine {
namespace Render {

    Threading::WorkgroupLocal<RenderContext *> sContext = nullptr;

    RenderContext::RenderContext(Threading::TaskQueue *queue)
        : mRenderQueue(queue)
        , mRenderThread(std::this_thread::get_id())        
    {
        assert(!sContext);
        sContext = this;
    }

    RenderContext::~RenderContext()
    {
        assert(mRenderTargets.empty());
        assert(sContext == this);
        sContext = nullptr;
    }

    void RenderContext::unloadAllResources()
    {

    }
    

    void RenderContext::addRenderTarget(RenderTarget *target)
    {
        mRenderTargets.push_back(target);
    }

    void RenderContext::removeRenderTarget(RenderTarget *target)
    {
        mRenderTargets.erase(std::find(mRenderTargets.begin(), mRenderTargets.end(), target));
    }

    void RenderContext::render()
    {
        beginFrame();
        for (RenderTarget *target : safeIterate(mRenderTargets))
            target->render();
        endFrame();
    }

    void RenderContext::beginFrame()
    {
        ++mFrame;
    }

    void RenderContext::endFrame()
    {
    }

    size_t RenderContext::frame() const
    {
        return mFrame;
    }

    void RenderContext::checkThread()
    {
        assert(mRenderThread == std::this_thread::get_id());
    }

    Threading::TaskQueue* RenderContext::renderQueue() {
        return sContext->mRenderQueue;
    }

    bool RenderContext::isRenderThread()
    {
        assert(sContext);
        return sContext->mRenderThread == std::this_thread::get_id();
    }

    RenderContext &RenderContext::getSingleton()
    {
        return *sContext;
    }

}
}