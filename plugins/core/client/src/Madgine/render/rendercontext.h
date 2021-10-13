#pragma once

#include "Meta/math/vector2i.h"

#include "Modules/threading/task.h"

#include "rendertextureconfig.h"

namespace Engine {
namespace Render {

    struct MADGINE_CLIENT_EXPORT RenderContext {
        RenderContext(Threading::TaskQueue *queue);
        virtual ~RenderContext();

        virtual std::unique_ptr<RenderTarget> createRenderWindow(Window::OSWindow *w, size_t samples = 1) = 0;
        virtual std::unique_ptr<RenderTarget> createRenderTexture(const Vector2i &size = { 1, 1 }, const RenderTextureConfig &config = {}) = 0;

        void addRenderTarget(RenderTarget *target);
        void removeRenderTarget(RenderTarget *target);

        void render();

        virtual void beginFrame();
        virtual void endFrame();

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

        size_t frame() const;

    protected:
        void checkThread();
        static void queueRenderTask(Threading::TaskHandle &&task);

        static bool isRenderThread();

    protected:
        std::vector<RenderTarget *> mRenderTargets;

        Threading::TaskQueue *mRenderQueue = nullptr;
        std::thread::id mRenderThread;

        size_t mFrame = 1;
    };

}
}


RegisterType(Engine::Render::RenderContext);