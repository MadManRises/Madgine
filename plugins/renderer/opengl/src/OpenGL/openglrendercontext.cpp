#include "opengllib.h"
#include "Madgine/gui/toplevelwindow.h"
#include "openglrenderwindow.h"

#include "Modules/debug/profiler/profiler.h"

#include "Modules/math/vector2i.h"
#include "Modules/math/vector3.h"
#include "Modules/math/vector3i.h"
#include "Modules/math/vector4.h"

#include "openglrendertexture.h"

#include "openglshaderloader.h"

#include "util/openglshader.h"

#include "Interfaces/window/windowapi.h"

#include "imagedata.h"

#include "openglfontloader.h"
#include "openglmeshloader.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "openglrendercontext.h"

RegisterType(Engine::Render::OpenGLRenderContext)

    UNIQUECOMPONENT(Engine::Render::OpenGLRenderContext)

        METATABLE_BEGIN(Engine::Render::OpenGLRenderContext)
            METATABLE_END(Engine::Render::OpenGLRenderContext)

                namespace Engine
{
    namespace Render {

        OpenGLRenderContext::OpenGLRenderContext()
        {
        }

        OpenGLRenderContext::~OpenGLRenderContext()
        {
        }

        std::unique_ptr<RenderTarget> OpenGLRenderContext::createRenderTexture(const Vector2i &size)
        {
            return std::make_unique<OpenGLRenderTexture>(this, size);
        }

        std::unique_ptr<RenderTarget> OpenGLRenderContext::createRenderWindow(Window::Window *w)
        {
            OpenGLRenderWindow *sharedContext = nullptr;
            for (RenderTarget *target : mRenderTargets) {
                sharedContext = dynamic_cast<OpenGLRenderWindow *>(target);
                if (sharedContext)
                    break;
			}

            return std::make_unique<OpenGLRenderWindow>(this, w, sharedContext);
        }

    }
}
