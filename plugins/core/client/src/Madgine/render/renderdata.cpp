#include "../clientlib.h"

#include "rendercontext.h"
#include "renderdata.h"

#include "Modules/threading/taskqueue.h"

namespace Engine {
namespace Render {

    Threading::TaskFuture<void> RenderData::update(RenderContext *context)
    {
        if (context->frame() != mFrame) {
            mFrame = context->frame();

            mCurrentFrame = context->renderQueue()->queueTask(render(context));
        }

        return mCurrentFrame;
    }

}
}