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

    RenderTarget::RenderTarget(RenderContext *context, bool global, std::string name, bool flipFlop, RenderTarget *blitSource)
        : mGlobal(global)
        , mName(std::move(name))
        , mFlipFlop(flipFlop)
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

        for (RenderPass *pass : mRenderPasses) {
            pushAnnotation(pass->name().data());
            for (size_t iteration = 0; iteration < pass->iterations(); ++iteration) {
                bool flipFlopping = pass->swapFlipFlopTextures(iteration);
                assert(mFlipFlop || !flipFlopping);
                size_t index = pass->targetIndex(iteration);
                size_t count = pass->targetCount(iteration);
                size_t subIndex = pass->targetSubresourceIndex(iteration);
                beginIteration(flipFlopping, index, count, subIndex);
                pass->render(this, iteration);
                endIteration();
                if (flipFlopping) {
                    for (size_t i = 0; i < count; ++i)
                        mFlipFlopIndices[index + i] ^= 1;
                }
            }
            popAnnotation();
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

    std::vector<const RenderPass *> RenderTarget::renderPasses() const
    {
        return { mRenderPasses.begin(), mRenderPasses.end() };
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

    void RenderTarget::beginIteration(bool flipFlopping, size_t targetIndex, size_t targetCount, size_t targetSubresourceIndex) const
    {
    }

    void RenderTarget::endIteration() const
    {
    }

    bool RenderTarget::canFlipFlop() const
    {
        return mFlipFlop;
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

    TextureDescriptor RenderTarget::texture(size_t index) const
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

    const RenderTarget *RenderTarget::blitSource() const
    {
        return mBlitSource;
    }

}
}