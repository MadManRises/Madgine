#include "../clientlib.h"

#include "rendercontext.h"

#include "Generic/container/safeiterator.h"

#include "Modules/threading/workgroupstorage.h"

#include "rendertarget.h"

#include "Madgine/fontloader/fontloader.h"

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
        for (std::pair<const std::string, FontLoader::Resource> &res : FontLoader::getSingleton()) {
            res.second.forceUnload();
        }
    }
    

    void RenderContext::addRenderTarget(RenderTarget *target)
    {
        mRenderTargets.push_back(target);
    }

    void RenderContext::removeRenderTarget(RenderTarget *target)
    {
        std::erase(mRenderTargets, target);
    }

    void RenderContext::render()
    {
        beginFrame();
        for (RenderTarget *target : safeIterate(mRenderTargets))
            target->update();
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