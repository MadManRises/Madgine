#pragma once

#include "Madgine/render/future.h"

namespace Engine {

namespace Render {

    struct MADGINE_CLIENT_EXPORT RenderData {

        RenderFuture update(RenderContext *context);

        RenderFuture lastFrame() const;

    protected:
        virtual RenderFuture render(RenderContext *context) = 0;

    private:
        size_t mFrame = 0;
        RenderFuture mLastFrame;
    };

}

}