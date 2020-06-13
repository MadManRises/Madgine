#pragma once

#include "openglrendertarget.h"

#include "util/openglprogram.h"
#include "util/opengltexture.h"
#include "util/openglvertexarray.h"
#include "util/openglbuffer.h"

#include "Modules/math/atlas2.h"

#include "imageloaderlib.h"
#include "imageloader.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLRenderWindow : OpenGLRenderTarget {
		OpenGLRenderWindow(OpenGLRenderContext *context, Window::Window *w, OpenGLRenderWindow *sharedContext = nullptr);
        ~OpenGLRenderWindow();

        virtual void beginFrame() override;
        virtual void endFrame() override;

		Texture *texture() const override;

		virtual bool resize(const Vector2i &size) override;
		virtual Vector2i size() const override;

        ContextHandle mContext = 0;
    private:

		Window::Window *mWindow;
        bool mReusedContext;
    };

}
}

RegisterType(Engine::Render::OpenGLRenderWindow);