#include "../../clientlib.h"

#include "blurpass.h"

#include "../rendertarget.h"

#include "../shadinglanguage/sl.h"

#include "Madgine/render/texturedescriptor.h"

#define SL_SHADER blur
#include "../shadinglanguage/sl_include.h"

namespace Engine {
namespace Render {

    BlurPass::BlurPass(int priority)
        : mPriority(priority)
    {
    }

    void BlurPass::setup(RenderTarget *target)
    {
        mPipeline.createStatic({ .vs = "blur", .ps = "blur", .bufferSizes = { sizeof(BlurData) } });
    }

    void BlurPass::shutdown()
    {
        mPipeline.reset();
    }

    void BlurPass::render(RenderTarget *target, size_t iteration)
    {
        if (iteration == 0) {
            target->pushAnnotation("Blur");
        }

        {
            auto data = mPipeline.mapParameters<BlurData>(0);
            data->horizontal = iteration % 2;
            data->textureSize = target->size();
        }

        if (iteration == 0) {
            target->bindTextures({ mInput->texture(mInputIndex) });
        } else {
            target->bindTextures({ target->texture(0, iteration) });
        }

        target->renderQuad(mPipeline);

        if (iteration == target->iterations() - 1)
            target->popAnnotation();
    }

    void BlurPass::preRender()
    {
        mInput->render();
    }

    void BlurPass::onTargetResize(const Vector2i &size)
    {
        mInput->resize(size);
    }

    int BlurPass::priority() const
    {
        return mPriority;
    }

    void BlurPass::setInput(RenderTarget *input, size_t inputIndex)
    {
        mInput = input;
        mInputIndex = inputIndex;
    }

}
}