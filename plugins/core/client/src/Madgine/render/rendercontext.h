#pragma once

#include "Meta/math/vector2i.h"

#include "rendertextureconfig.h"

#include "Modules/uniquecomponent/uniquecomponent.h"

#include "Modules/threading/taskfuture.h"

namespace Engine {
namespace Render {

    struct MADGINE_CLIENT_EXPORT RenderContext {
        RenderContext(Threading::TaskQueue *queue);
        virtual ~RenderContext();

        virtual std::unique_ptr<RenderTarget> createRenderWindow(Window::OSWindow *w, size_t samples = 1) = 0;
        virtual std::unique_ptr<RenderTarget> createRenderTexture(const Vector2i &size = { 1, 1 }, const RenderTextureConfig &config = {}) = 0;

        virtual Threading::Task<void> unloadAllResources();

        void addRenderTarget(RenderTarget *target);
        void removeRenderTarget(RenderTarget *target);

        Threading::Task<void> render();

        virtual bool beginFrame();
        virtual void endFrame();

		static RenderContext &getSingleton();

        size_t frame() const;

        static Threading::TaskQueue *renderQueue();

        virtual bool supportsMultisampling() const = 0;

    protected:
        void checkThread();

        static bool isRenderThread();
    protected:
        std::vector<RenderTarget *> mRenderTargets;

        Threading::TaskQueue *mRenderQueue = nullptr;
        std::thread::id mRenderThread;

        size_t mFrame = 1;
    };

}
}


REGISTER_TYPE(Engine::Render::RenderContext)