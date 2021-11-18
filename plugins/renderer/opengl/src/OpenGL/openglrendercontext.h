#pragma once

#include "Madgine/render/rendercontextcollector.h"
#include "Madgine/render/rendercontext.h"
#include "Modules/uniquecomponent/uniquecomponent.h"

namespace Engine {
namespace Render {

    ContextHandle createContext(Window::OSWindow *window, size_t samples, ContextHandle reusedContext, bool setup = true);
    void destroyContext(Window::OSWindow *window, ContextHandle context, bool reusedContext = false);
    void makeCurrent(Window::OSWindow *window, ContextHandle context);

    struct MADGINE_OPENGL_EXPORT OpenGLRenderContext : public RenderContextComponent<OpenGLRenderContext> {
        OpenGLRenderContext(Threading::TaskQueue *queue);
        ~OpenGLRenderContext();

        virtual std::unique_ptr<RenderTarget> createRenderWindow(Window::OSWindow *w, size_t samples) override;
        virtual std::unique_ptr<RenderTarget> createRenderTexture(const Vector2i &size = { 1, 1 }, const RenderTextureConfig &config = {}) override;

		static OpenGLRenderContext &getSingleton();

        virtual bool supportsMultisampling() const override;
    };

}
}

RegisterType(Engine::Render::OpenGLRenderContext);