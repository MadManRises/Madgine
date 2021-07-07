#pragma once

#include "Meta/math/vector2i.h"

#include "Modules/threading/task.h"

#include "rendertextureconfig.h"

namespace Engine {
namespace Render {

    struct MADGINE_CLIENT_EXPORT RenderContext {
        RenderContext(Threading::TaskQueue *queue);
        virtual ~RenderContext();

        virtual std::unique_ptr<RenderTarget> createRenderWindow(Window::OSWindow *w) = 0;
        virtual std::unique_ptr<RenderTarget> createRenderTexture(const Vector2i &size = { 1, 1 }, const RenderTextureConfig &config = {}) = 0;

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

		static RenderContext &getSingleton();

    protected:
        void checkThread();
        static void queueRenderTask(Threading::TaskHandle &&task);

        static bool isRenderThread();

    protected:
        std::vector<RenderTarget *> mRenderTargets;

        Threading::TaskQueue *mRenderQueue = nullptr;
        std::thread::id mRenderThread;
    };

}
}


RegisterType(Engine::Render::RenderContext);