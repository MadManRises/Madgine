#include "../../clientlib.h"

#include "bloompass.h"

#include "../rendercontext.h"
#include "../rendertarget.h"

#include "../shadinglanguage/sl_support_begin.h"
#include "shaders/bloom.sl"
#include "../shadinglanguage/sl_support_end.h"

#include "Meta/keyvalue/metatable_impl.h"

METATABLE_BEGIN(Engine::Render::BloomPass)
MEMBER(mExposure)
METATABLE_END(Engine::Render::BloomPass)

namespace Engine {
namespace Render {

    BloomPass::BloomPass(int priority)
        : mPriority(priority)
        , mBlur(50)
    {
    }

    BloomPass::~BloomPass()
    {
    }

    void BloomPass::setup(RenderTarget *target)
    {
        mPipeline.create({ .vs = "bloom", .ps = "bloom", .bufferSizes = { sizeof(BloomData) } });

        mBlurTarget = target->context()->createRenderTexture(target->size(), { .mIterations = 10, .mFormat = FORMAT_RGBA16F });

        mBlurTarget->addRenderPass(&mBlur);

        addDependency(mBlurTarget.get());
    }

    void BloomPass::shutdown()
    {
        removeDependency(mBlurTarget.get());

        mBlurTarget.reset();

        mPipeline.reset();
    }

    void BloomPass::render(RenderTarget *target, size_t iteration)
    {
        if (!mPipeline.available())
            return;

        if (iteration == 0)
            target->context()->pushAnnotation("Bloom");

        mPipeline->bindTextures({ mInput->texture(mInputIndex), mBlurTarget->texture(0) });

        mPipeline->mapParameters<BloomData>(0)->exposure = mExposure;

        mPipeline->renderQuad();

        if (iteration == target->iterations() - 1)
            target->context()->popAnnotation();
    }

    void BloomPass::onTargetResize(const Vector2i &size)
    {
        mBlurTarget->resize(size);
        mInput->resize(size);
    }

    int BloomPass::priority() const
    {
        return mPriority;
    }

    void BloomPass::setInput(RenderTarget *input, size_t inputIndex, RenderTarget *blurInput, size_t blurIndex)
    {
        mInput = input;
        addDependency(input);
        mInputIndex = inputIndex;
        mBlur.setInput(blurInput ? blurInput : input, blurIndex);
    }

}
}