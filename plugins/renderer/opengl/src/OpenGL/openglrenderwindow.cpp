#include "opengllib.h"
#include "openglrenderwindow.h"

#include "Modules/debug/profiler/profile.h"

#include "Interfaces/window/windowapi.h"
#include "Interfaces/window/windowsettings.h"

#include "openglmeshloader.h"
#include "openglprogramloader.h"
#include "openglshaderloader.h"
#include "opengltextureloader.h"

#include "openglrendercontext.h"

#if LINUX
#include <GL/glx.h>
#include <X11/Xlib.h>
    namespace Engine {
    namespace Window {
        extern Display *sDisplay();
    }
    }
#elif ANDROID || EMSCRIPTEN
#    include <EGL/egl.h>
namespace Engine {
namespace Window {
    extern EGLDisplay sDisplay;
}
}
#endif

namespace Engine {
namespace Render {

#if OPENGL_ES
    THREADLOCAL(OpenGLSSBOBufferStorage *)
    OpenGLRenderWindow::sCurrentSSBOBuffer = nullptr;
#endif

    OpenGLRenderWindow::OpenGLRenderWindow(OpenGLRenderContext *context, Window::OSWindow *w, size_t samples, OpenGLRenderWindow *reusedResources)
        : OpenGLRenderTarget(context, true, w->title())
        , mOsWindow(w)
        , mReusedContext(reusedResources)
    {
        ContextHandle reusedContext = nullptr;
        if (reusedResources) {
            reusedContext = reusedResources->mContext;
        }

        mContext = createContext(w, context->supportsMultisampling() ? samples : 1, reusedContext);

#if OPENGL_ES
        mSSBOBuffer = { 3, 128 };
#endif
    }

    OpenGLRenderWindow::~OpenGLRenderWindow()
    {
#if OPENGL_ES
        mSSBOBuffer.reset();
#endif
        destroyContext(mOsWindow, mContext, mReusedContext);
    }

    void OpenGLRenderWindow::beginIteration(size_t iteration)
    {
        PROFILE();

        Engine::Render::makeCurrent(mOsWindow, mContext);

#if OPENGL_ES
        sCurrentSSBOBuffer = &*mSSBOBuffer;
#endif

        OpenGLRenderTarget::beginIteration(iteration);
    }

    void OpenGLRenderWindow::endIteration(size_t iteration)
    {
        OpenGLRenderTarget::endIteration(iteration);

#if WINDOWS
        SwapBuffers(GetDC((HWND)mOsWindow->mHandle));
#elif LINUX
        glXSwapBuffers(Window::sDisplay(), mOsWindow->mHandle);
#elif ANDROID || EMSCRIPTEN
        eglSwapBuffers(Window::sDisplay, (EGLSurface)mOsWindow->mHandle);
#elif OSX
        OSXBridge::swapBuffers(mContext);
#elif IOS
        IOSBridge::swapBuffers(mContext);
#else
#    error "Unsupported Platform!"
#endif
    }

    TextureDescriptor OpenGLRenderWindow::texture(size_t index, size_t iteration) const
    {
        return {};
    }

    size_t OpenGLRenderWindow::textureCount() const
    {
        return 0;
    }

    TextureDescriptor OpenGLRenderWindow::depthTexture() const
    {
        return {};
    }

    Vector2i OpenGLRenderWindow::size() const
    {
        InterfacesVector size = mOsWindow->renderSize();
        return { size.x, size.y };
    }

    bool OpenGLRenderWindow::resizeImpl(const Vector2i &size)
    {
        return true;
    }

}
}
