#include "opengllib.h"
#include "openglrenderwindow.h"

#include "Modules/debug/profiler/profile.h"

#include "Interfaces/window/windowapi.h"
#include "Interfaces/window/windowsettings.h"

#include "openglmeshloader.h"
#include "openglprogramloader.h"
#include "openglshaderloader.h"
#include "opengltextureloader.h"

#if WINDOWS
typedef HGLRC(WINAPI *PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC hDC, HGLRC hShareContext, const int *attribList);
typedef BOOL(WINAPI *PFNWGLSWAPINTERVALEXTPROC)(int interval);

static PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
static PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = nullptr;

#elif LINUX
#    include <GL/glx.h>
#    include <X11/Xlib.h>
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
#elif OSX
#    include "osxopengl.h"
#elif IOS
#    include "iosopengl.h"
#endif

namespace Engine {
namespace Render {

#if OPENGL_ES
    THREADLOCAL(OpenGLSSBOBufferStorage *)
    OpenGLRenderWindow::sCurrentSSBOBuffer = nullptr;
#endif

#if !ANDROID && !EMSCRIPTEN && !OSX && !IOS
    static void __stdcall glDebugOutput(GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei length,
        const GLchar *message,
        const void *userParam)
    {
        // ignore non-significant error/warning codes
        if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
            return;

        Util::MessageType lvl;
        switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            lvl = Util::MessageType::ERROR_TYPE;
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            lvl = Util::MessageType::WARNING_TYPE;
            break;
        case GL_DEBUG_SEVERITY_LOW:
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            lvl = Util::MessageType::LOG_TYPE;
            break;
        }

        Util::LogDummy cout(lvl);
        cout << "Debug message (" << id << "): " << message << "\n";

        switch (source) {
        case GL_DEBUG_SOURCE_API:
            cout << "Source: API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            cout << "Source: Window System";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            cout << "Source: Shader Compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            cout << "Source: Third Party";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            cout << "Source: Application";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            cout << "Source: Other";
            break;
        }
        cout << "\n";

        switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            cout << "Type: Error";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            cout << "Type: Deprecated Behaviour";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            cout << "Type: Undefined Behaviour";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            cout << "Type: Portability";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            cout << "Type: Performance";
            break;
        case GL_DEBUG_TYPE_MARKER:
            cout << "Type: Marker";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            cout << "Type: Push Group";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            cout << "Type: Pop Group";
            break;
        case GL_DEBUG_TYPE_OTHER:
            cout << "Type: Other";
            break;
        }
        cout << "\n";
    }
#endif

    void resetContext()
    {
        GL_LOG("Resetting Context");
#if WINDOWS
        wglMakeCurrent(NULL, NULL);
#elif LINUX
        glXMakeCurrent(Window::sDisplay(), None, NULL);
#elif ANDROID || EMSCRIPTEN
        eglMakeCurrent(Window::sDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
#elif OSX
        OSXBridge::resetContext();
#endif
    }

    void makeCurrent(Window::OSWindow *window, ContextHandle context)
    {
        GL_LOG("Setting Context: " << context);
#if WINDOWS
        HDC windowDC = GetDC((HWND)window->mHandle);
        if (!wglMakeCurrent(windowDC, context)) {
            LOG_ERROR("Error-Code: " << GetLastError());
            std::terminate();
        }
#elif LINUX
        if (!glXMakeCurrent(Window::sDisplay(), window->mHandle, context)) {
            LOG_ERROR("Error-Code: " << errno);
            std::terminate();
        }
#elif ANDROID || EMSCRIPTEN
        EGLSurface surface = (EGLSurface)window->mHandle;
        if (!eglMakeCurrent(Window::sDisplay, surface, surface, context)) {
            LOG_ERROR("Error-Code: " << errno);
            std::terminate();
        }
#elif OSX
        OSXBridge::makeCurrent(context);
#endif
    }

    ContextHandle setupWindowInternal(Window::OSWindow *window, ContextHandle reusedContext)
    {

#if WINDOWS
        PIXELFORMATDESCRIPTOR pfd = {
            sizeof(PIXELFORMATDESCRIPTOR),
            1,
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, //Flags
            PFD_TYPE_RGBA, // The kind of framebuffer. RGBA or palette.
            32, // Colordepth of the framebuffer.
            0, 0, 0, 0, 0, 0,
            0,
            0,
            0,
            0, 0, 0, 0,
            24, // Number of bits for the depthbuffer
            8, // Number of bits for the stencilbuffer
            0, // Number of Aux buffers in the framebuffer.
            PFD_MAIN_PLANE,
            0,
            0, 0, 0
        };

        HDC windowDC = GetDC((HWND)window->mHandle);

        int format = ChoosePixelFormat(windowDC, &pfd);
        SetPixelFormat(windowDC, format, &pfd);

        HGLRC context;
        if (reusedContext) {
            context = reusedContext;
        } else {
            if (wglCreateContextAttribsARB) {

#    define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126

                int attribs[] = {
                    WGL_CONTEXT_PROFILE_MASK_ARB, GL_TRUE,
                    0
                };

                context = wglCreateContextAttribsARB(windowDC, NULL, attribs);
            } else {
                context = wglCreateContext(windowDC);
            }
        }

#elif LINUX
        GLXContext context;
        if (reusedContext) {
            context = reusedContext;
        } else {
            static GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };

            static XVisualInfo *vi = glXChooseVisual(Window::sDisplay(), 0, att);
            assert(vi);

            context = glXCreateContext(Window::sDisplay(), vi, /*sharedContext*/ nullptr, GL_TRUE);
        }

#elif ANDROID || EMSCRIPTEN

        EGLContext context;
        if (reusedContext) {
            context = reusedContext;
        } else {
            const EGLint attribs[] = {
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_BLUE_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_RED_SIZE, 8,
                EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                EGL_CONFORMANT, EGL_OPENGL_ES2_BIT,
                EGL_NONE
            };

            const EGLint contextAttribs[] = {
                EGL_CONTEXT_CLIENT_VERSION, 3,
                EGL_NONE
            };

            EGLConfig config;
            EGLint numConfigs;

            if (!eglChooseConfig(Window::sDisplay, attribs, &config, 1, &numConfigs))
                std::terminate();

            context = eglCreateContext(Window::sDisplay, config, /*sharedContext*/ nullptr, contextAttribs);
        }

#elif OSX

        ContextHandle context;
        if (reusedContext) {
            context = reusedContext;
        } else {
            context = OSXBridge::createContext(window);
        }

#elif IOS

        ContextHandle context;
        if (reusedContext) {
            context = reusedContext;
        } else {
            context = IOSBridge::createContext(window);
        }

#endif

        if (!reusedContext) {
            makeCurrent(window, context);

#if WINDOWS
            if (wglSwapIntervalEXT)
                wglSwapIntervalEXT(0);
#endif

            if (glGetString) {
                const GLubyte *val;
#define GL_LOG_PROPERTY(name)         \
    val = glGetString(name);          \
    if (val) {                        \
        LOG_DEBUG(#name ": " << val); \
    }

                GL_LOG_PROPERTY(GL_VERSION);
                GL_LOG_PROPERTY(GL_VENDOR);
                GL_LOG_PROPERTY(GL_RENDERER);
                GL_LOG_PROPERTY(GL_EXTENSIONS);
            }
        }

        return context;
    }

    void shutdownWindow(Window::OSWindow *window, ContextHandle context, bool reusedContext = false)
    {
        if (!reusedContext)
            resetContext();

#if WINDOWS
        HDC device = GetDC((HWND)window->mHandle);

        ReleaseDC((HWND)window->mHandle, device);

        if (!reusedContext)
            wglDeleteContext(context);
#elif LINUX
        if (!reusedContext)
            glXDestroyContext(Window::sDisplay(), context);
#elif ANDROID || EMSCRIPTEN
        if (!reusedContext)
            eglDestroyContext(Window::sDisplay, context);
#elif OSX
        if (!reusedContext)
            OSXBridge::destroyContext(context);
#endif
    }

#if !ANDROID && !EMSCRIPTEN && !IOS
    namespace {
        void OpenGLInit()
        {
            static std::mutex sMutex;
            static bool init = false;

            std::lock_guard guard(sMutex);
            if (!init) {
                Engine::Window::WindowSettings settings;
                settings.mHidden = true;
                Window::OSWindow *tmp = Window::sCreateWindow(settings);
                ContextHandle context = setupWindowInternal(tmp, nullptr);

#    if WINDOWS
                wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));
                assert(wglCreateContextAttribsARB);
                wglSwapIntervalEXT = reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(wglGetProcAddress("wglSwapIntervalEXT"));
#    endif
                bool result = gladLoadGL();
                assert(result);

                shutdownWindow(tmp, context);
                tmp->destroy();
                init = true;
            }
        }
    }
#endif

    OpenGLRenderWindow::OpenGLRenderWindow(OpenGLRenderContext *context, Window::OSWindow *w, OpenGLRenderWindow *reusedResources)
        : OpenGLRenderTarget(context)
        , mOsWindow(w)
        , mReusedContext(reusedResources)
    {
#if !ANDROID && !EMSCRIPTEN && !IOS
        OpenGLInit();
#endif

        ContextHandle reusedContext = nullptr;
        if (reusedResources) {
            reusedContext = reusedResources->mContext;
        }

        mContext = setupWindowInternal(w, reusedContext);

#if !ANDROID && !EMSCRIPTEN && !OSX && !IOS
        glEnable(GL_DEBUG_OUTPUT);
        GL_CHECK();
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        GL_CHECK();
        glDebugMessageCallback(glDebugOutput, nullptr);
        GL_CHECK();
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        GL_CHECK();
#endif

        glVertexAttrib3f(0, 0, 0, 0);
        glVertexAttrib2f(1, 0, 0);
        glVertexAttrib4f(2, 1, 1, 1, 1);
        glVertexAttrib2f(4, 0, 0);
        glVertexAttribI4i(5, 0, 0, 0, 0);
        glVertexAttrib4f(6, 0, 0, 0, 0);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
        //glDepthRange(0.0, 1.0);

#if OPENGL_ES
        mSSBOBuffer = { 3, 128 };
#endif
    }

    OpenGLRenderWindow::~OpenGLRenderWindow()
    {
#if OPENGL_ES
        mSSBOBuffer.reset();
#endif
        shutdownWindow(mOsWindow, mContext, mReusedContext);
    }

    void OpenGLRenderWindow::beginFrame()
    {
        PROFILE();

        Engine::Render::makeCurrent(mOsWindow, mContext);

#if OPENGL_ES
        sCurrentSSBOBuffer = &mSSBOBuffer;
#endif

        OpenGLRenderTarget::beginFrame();
    }

    void OpenGLRenderWindow::endFrame()
    {
        OpenGLRenderTarget::endFrame();

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

    TextureHandle OpenGLRenderWindow::texture() const
    {
        return 0;
    }

    TextureHandle OpenGLRenderWindow::depthTexture() const
    {
        return 0;
    }

    Vector2i OpenGLRenderWindow::size() const
    {
        InterfacesVector size = mOsWindow->renderSize();
        return { size.x, size.y };
    }

    bool OpenGLRenderWindow::resize(const Vector2i &size)
    {
        return true;
    }

}
}
