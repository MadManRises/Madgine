#include "../clientlib.h"

#include "rendercontext.h"
#include "renderdata.h"

#include "Modules/threading/taskqueue.h"

namespace Engine {
namespace Render {

    RenderFuture RenderData::update(RenderContext *context)
    {
        if (context->frame() != mFrame) {
            mFrame = context->frame();

            mLastFrame = render(context);
        }

        return mLastFrame;
    }

    RenderFuture RenderData::lastFrame() const
    {
        return mLastFrame;
    }

}
}