#include "opengllib.h"
#include "Madgine/window/mainwindow.h"
#include "openglrenderwindow.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "openglrendercontext.h"
#include "openglrendertexture.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

    UNIQUECOMPONENT(Engine::Render::OpenGLRenderContext)

        METATABLE_BEGIN(Engine::Render::OpenGLRenderContext)
            METATABLE_END(Engine::Render::OpenGLRenderContext)

                namespace Engine
{
    namespace Render {



        OpenGLRenderContext::OpenGLRenderContext(Threading::TaskQueue *queue) : UniqueComponent(queue)
        {
        }

        OpenGLRenderContext::~OpenGLRenderContext()
        {
        }

        std::unique_ptr<RenderTarget> OpenGLRenderContext::createRenderTexture(const Vector2i &size)
        {
            return std::make_unique<OpenGLRenderTexture>(this, size);
        }

        OpenGLRenderContext &OpenGLRenderContext::getSingleton()
        {
            return static_cast<OpenGLRenderContext &>(RenderContext::getSingleton());
        }

        std::unique_ptr<RenderTarget> OpenGLRenderContext::createRenderWindow(Window::Window *w)
        {
            checkThread();

            OpenGLRenderWindow *sharedContext = nullptr;
            for (RenderTarget *target : mRenderTargets) {
                sharedContext = dynamic_cast<OpenGLRenderWindow *>(target);
                if (sharedContext)
                    break;
			}

            std::unique_ptr<RenderTarget> window = std::make_unique<OpenGLRenderWindow>(this, w, sharedContext);

			#if OPENGL_ES
            mSSBOBuffer = { 3, 128 };
			#endif

			return window;
        }

    }
}
