#pragma once

#include "Modules/threading/task.h"

namespace Engine {

namespace Render {

    struct MADGINE_CLIENT_EXPORT RenderData {

        Threading::TaskFuture<void> update(RenderContext *context);

    protected:
        virtual Threading::ImmediateTask<void> render(RenderContext *context) = 0;

    private:
        size_t mFrame = 0;
        Threading::TaskFuture<void> mCurrentFrame;
    };

}

}