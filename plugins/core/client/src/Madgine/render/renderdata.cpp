#include "../clientlib.h"

#include "rendercontext.h"
#include "renderdata.h"

namespace Engine {
namespace Render {

    RenderData::RenderData(RenderContext *context)
        : mContext(context)
    {
    }

    void RenderData::update()
    {
        if (mContext->frame() == mFrame)
            return;
        mFrame = mContext->frame();

        render();
    }

    RenderContext *RenderData::context() const
    {
        return mContext;
    }

}
}