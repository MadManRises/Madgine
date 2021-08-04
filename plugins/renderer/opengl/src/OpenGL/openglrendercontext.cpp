#include "opengllib.h"
#include "Madgine/window/mainwindow.h"
#include "openglrenderwindow.h"

#include "Meta/keyvalue/metatable_impl.h"
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

        std::unique_ptr<RenderTarget> OpenGLRenderContext::createRenderTexture(const Vector2i &size, const RenderTextureConfig &config)
        {
            return std::make_unique<OpenGLRenderTexture>(this, size, config);
        }

        OpenGLRenderContext &OpenGLRenderContext::getSingleton()
        {
            return static_cast<OpenGLRenderContext &>(RenderContext::getSingleton());
        }

        std::unique_ptr<RenderTarget> OpenGLRenderContext::createRenderWindow(Window::OSWindow *w)
        {
            checkThread();

            OpenGLRenderWindow *sharedContext = nullptr;
            for (RenderTarget *target : mRenderTargets) {
                sharedContext = dynamic_cast<OpenGLRenderWindow *>(target);
                if (sharedContext)
                    break;
			}

            std::unique_ptr<RenderTarget> window = std::make_unique<OpenGLRenderWindow>(this, w, sharedContext);

			return window;
        }

    }
}
