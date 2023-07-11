#include "../clientlib.h"

#include "rendercontext.h"
#include "renderpass.h"
#include "rendertarget.h"

#include "Meta/math/matrix4.h"

#include "Meta/keyvalue/metatable_impl.h"

METATABLE_BEGIN(Engine::Render::RenderTarget)
METATABLE_END(Engine::Render::RenderTarget)

namespace Engine {
namespace Render {

    RenderTarget::RenderTarget(RenderContext *context, bool global, std::string name, size_t iterations, RenderTarget *blitSource)
        : mGlobal(global)
        , mName(std::move(name))
        , mIterations(iterations)
        , mBlitSource(blitSource)
        , mContext(context)
    {
        if (global)
            context->addRenderTarget(this);
    }

    RenderTarget::~RenderTarget()
    {
        for (RenderPass *pass : mRenderPasses) {
            pass->shutdown();
        }
        if (mGlobal)
            context()->removeRenderTarget(this);
    }

    Threading::ImmediateTask<void> RenderTarget::render(RenderContext *context)
    {
        if (skipFrame())
            co_return;

        std::vector<Threading::TaskFuture<void>> dependencies;

        if (mBlitSource)
            dependencies.push_back(mBlitSource->update(context));

        for (RenderPass *pass : mRenderPasses)
            pass->preRender(dependencies, context);

        for (Threading::TaskFuture<void> &dependency : dependencies)
            co_await dependency;

        beginFrame();

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

    bool RenderTarget::skipFrame()
    {
        return false;
    }

    void RenderTarget::beginFrame()
    {
        pushAnnotation(mName.empty() ? "<unnamed target>" : mName.c_str());
    }

    void RenderTarget::endFrame()
    {
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

    const std::string &RenderTarget::name() const
    {
        return mName;
    }

    bool RenderTarget::resize(const Vector2i &size)
    {
        if (mBlitSource)
            mBlitSource->resize(size);
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

    TextureDescriptor RenderTarget::texture(size_t index, size_t iteration) const
    {
        return {};
    }

    size_t RenderTarget::textureCount() const
    {
        return 0;
    }

    TextureDescriptor RenderTarget::depthTexture() const
    {
        return {};
    }

    RenderContext *RenderTarget::context() const
    {
        return mContext;
    }

}
}