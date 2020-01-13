#pragma once

#include "Madgine/render/rendercontextcollector.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLRenderContext : public RenderContextComponent<OpenGLRenderContext> {
        OpenGLRenderContext(SignalSlot::TaskQueue *queue);
        ~OpenGLRenderContext();

		virtual std::unique_ptr<RenderTarget> createRenderWindow(Window::Window *w) override;
        virtual std::unique_ptr<RenderTarget> createRenderTexture(const Vector2i &size = { 1, 1 }) override;

    };

}
}