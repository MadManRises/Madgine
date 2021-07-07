#pragma once

#include "Madgine/render/rendercontextcollector.h"
#include "Madgine/render/rendercontext.h"
#include "Modules/uniquecomponent/uniquecomponent.h"

#include "util/openglssbobuffer.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLRenderContext : public RenderContextComponent<OpenGLRenderContext> {
        OpenGLRenderContext(Threading::TaskQueue *queue);
        ~OpenGLRenderContext();

        virtual std::unique_ptr<RenderTarget> createRenderWindow(Window::OSWindow *w) override;
        virtual std::unique_ptr<RenderTarget> createRenderTexture(const Vector2i &size = { 1, 1 }, const RenderTextureConfig &config = {}) override;

		static OpenGLRenderContext &getSingleton();

#if OPENGL_ES    
        OpenGLSSBOBufferStorage mSSBOBuffer;
#endif
    };

}
}

RegisterType(Engine::Render::OpenGLRenderContext);