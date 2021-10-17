#include "../../clientlib.h"

#include "blurpass.h"

#include "../rendertarget.h"

#include "../shadinglanguage/sl.h"

#include "render/texturedescriptor.h"

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
        mProgram.create("blur", { sizeof(BlurData) });        
    }

    void BlurPass::shutdown()
    {
        mProgram.reset();
    }

    void BlurPass::render(RenderTarget *target, size_t iteration)
    {
        if (iteration == 0) {
            target->pushAnnotation("Blur");            
        }

        {
            auto data = mProgram.mapParameters(0).cast<BlurData>();
            data->horizontal = iteration % 2;
            data->textureSize = target->size();
        }

        if (iteration == 0) {         
            target->bindTextures({ mInput->texture(mInputIndex) });
        } else {
            target->bindTextures({ target->texture(0, iteration) });
        }

        target->renderQuad(mProgram);

        if (iteration == target->iterations() - 1)
            target->popAnnotation();
    }

    void BlurPass::preRender()
    {
        mInput->render();
    }

    void BlurPass::onResize(const Vector2i &size)
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