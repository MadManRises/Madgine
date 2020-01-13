#pragma once

#include "Modules/math/vector2i.h"

#include "Modules/signalslot/task.h"

namespace Engine {
namespace Render {

    struct MADGINE_CLIENT_EXPORT RenderContext {
        RenderContext(SignalSlot::TaskQueue *queue);
        virtual ~RenderContext();

        virtual std::unique_ptr<RenderTarget> createRenderWindow(Window::Window *w) = 0;
        virtual std::unique_ptr<RenderTarget> createRenderTexture(const Vector2i &size = { 1, 1 }) = 0;

        void addRenderTarget(RenderTarget *target);
        void removeRenderTarget(RenderTarget *target);

        void render();

        template <typename F>
        static bool execute(F &&f)
        {
            if (isRenderThread()) {
                std::forward<F>(f)();
                return true;
            } else {
                queueRenderTask(std::forward<F>(f));
                return false;
            }
        }

    protected:
        void checkThread();
        static void queueRenderTask(SignalSlot::TaskHandle &&task);

        static bool isRenderThread();

    protected:
        std::vector<RenderTarget *> mRenderTargets;

        SignalSlot::TaskQueue *mRenderQueue = nullptr;
        std::thread::id mRenderThread;
    };

}
}