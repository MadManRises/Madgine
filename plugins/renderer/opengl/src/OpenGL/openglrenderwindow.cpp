#include "opengllib.h"
#include "openglrenderwindow.h"

#include "Interfaces/window/windowapi.h"

#include "openglrendercontext.h"

#if ANDROID || EMSCRIPTEN
#    include <EGL/egl.h>
extern EGLDisplay sDisplay;

#    if ANDROID
#        include <android/native_window.h>
#    endif
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

#if ANDROID || EMSCRIPTEN

        assert(sDisplay);

        const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_CONFORMANT, EGL_OPENGL_ES2_BIT,
            /* EGL_SAMPLE_BUFFERS, 1,*/
            EGL_NONE
        };

        EGLConfig config;
        EGLint numConfigs;
        EGLint format;

        if (!eglChooseConfig(sDisplay, attribs, &config, 1, &numConfigs))
            throw 0;

        if (!eglGetConfigAttrib(sDisplay, config, EGL_NATIVE_VISUAL_ID, &format))
            throw 0;

#    if ANDROID
        ANativeWindow_setBuffersGeometry((ANativeWindow *)w->mHandle, 0, 0, format);

        mSurface = eglCreateWindowSurface(sDisplay, config, (ANativeWindow *)w->mHandle, 0);
        assert(mSurface);

#    endif

#endif

        mContext = createContext(getSurface(), context->supportsMultisampling() ? samples : 1, reusedContext);

#if OPENGL_ES
        mSSBOBuffer = { 3, 128 };
#endif
    }

    OpenGLRenderWindow::~OpenGLRenderWindow()
    {
#if OPENGL_ES
        mSSBOBuffer.reset();
#endif

        destroyContext(getSurface(), mContext, mReusedContext);

#if ANDROID || EMSCRIPTEN
        eglDestroySurface(sDisplay, mSurface);
#endif
    }

    void OpenGLRenderWindow::beginIteration(bool flipFlopping, size_t targetIndex, size_t targetCount, size_t targetSubresourceIndex) const
    {
        
        OpenGLRenderTarget::beginIteration(flipFlopping, targetIndex, targetCount, targetSubresourceIndex);        

    }

    void OpenGLRenderWindow::endIteration() const
    {
        OpenGLRenderTarget::endIteration();        
    }

    bool OpenGLRenderWindow::skipFrame()
    {
        if (mOsWindow->isMinimized())
            return true;
        return false;
    }

    void OpenGLRenderWindow::beginFrame()
    {
        makeCurrent(getSurface(), mContext);

#if OPENGL_ES
        sCurrentSSBOBuffer = &*mSSBOBuffer;
#endif

        OpenGLRenderTarget::beginFrame();
    }

    void OpenGLRenderWindow::endFrame()
    {
        OpenGLRenderTarget::endFrame();

        swapBuffers(getSurface(), mContext);
    }

    Vector2i OpenGLRenderWindow::size() const
    {
        InterfacesVector size = mOsWindow->renderSize();
        return { size.x, size.y };
    }

    SurfaceHandle OpenGLRenderWindow::getSurface() const
    {
#if WINDOWS
        return GetDC((HWND)mOsWindow->mHandle);
#elif LINUX
        return mOsWindow->mHandle;
#elif ANDROID || EMSCRIPTEN
        return mSurface;
#elif OSX
        return mOsWindow;
#elif IOS
        return mOsWindow;
#else
#    error "Unsupported Platform!"
#endif
    }

    bool OpenGLRenderWindow::resizeImpl(const Vector2i &size)
    {
        return true;
    }

}
}
