#include "../../clientlib.h"

#include "blurpass.h"

#include "../rendertarget.h"

#include "Madgine/render/texturedescriptor.h"

#include "../rendercontext.h"


#include "../shadinglanguage/sl_support_begin.h"
#include "shaders/blur.sl"
#include "../shadinglanguage/sl_support_end.h"

namespace Engine {
namespace Render {

    BlurPass::BlurPass(int priority)
        : mPriority(priority)
    {
    }

    void BlurPass::setup(RenderTarget *target)
    {
        mPipeline.create({ .vs = "blur", .ps = "blur", .bufferSizes = { sizeof(BlurData) } });
    }

    void BlurPass::shutdown()
    {
        mPipeline.reset();
    }

    void BlurPass::render(RenderTarget *target, size_t iteration)
    {
        if (!mPipeline.available())
            return;

        if (iteration == 0) {
            target->pushAnnotation("Blur");
        }

        {
            auto data = mPipeline->mapParameters<BlurData>(0);
            data->horizontal = iteration % 2;
            data->textureSize = mInput->size();
        }

        if (iteration == 0) {
            mPipeline->bindTextures(target, { mInput->texture(mInputIndex) });
        } else {
            mPipeline->bindTextures(target, { target->texture(0, iteration) });
        }

        mPipeline->renderQuad(target);

        if (iteration == target->iterations() - 1)
            target->popAnnotation();
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
        addDependency(input);
        mInputIndex = inputIndex;
    }

}
}