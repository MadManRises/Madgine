#include "opengllib.h"

#include "openglrenderer.h"

#include "gui/widgets/toplevelwindow.h"

#include "Interfaces/window/windowapi.h"

#include "openglrenderwindow.h"

#include "Madgine/scene/camera.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"

#if WINDOWS
typedef HGLRC(WINAPI *PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC hDC, HGLRC hShareContext, const int *attribList);
typedef BOOL(WINAPI *PFNWGLSWAPINTERVALEXTPROC)(int interval);

static PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
static PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = nullptr;

#elif LINUX
#include <GL/glx.h>
#include <X11/Xlib.h>
namespace Engine {
namespace Window {
    extern Display *sDisplay();
}
}
#elif ANDROID || EMSCRIPTEN
#include <EGL/egl.h>
namespace Engine {
namespace Window {
    extern EGLDisplay sDisplay;
}
}
#endif

UNIQUECOMPONENT(Engine::Render::OpenGLRenderer);

namespace Engine {

namespace Render {

    ContextHandle setupWindowInternal(Window::Window *window, ContextHandle sharedContext)
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
        if (wglCreateContextAttribsARB) {

#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126

            int attribs[] = {
                WGL_CONTEXT_PROFILE_MASK_ARB, GL_TRUE,
                0
            };

            context = wglCreateContextAttribsARB(windowDC, NULL, attribs);
        } else {
            context = wglCreateContext(windowDC);
        }

		if (sharedContext)
            wglShareLists(sharedContext, context);

#elif LINUX
        static GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };

        static XVisualInfo *vi = glXChooseVisual(Window::sDisplay(), 0, att);
        assert(vi);

        GLXContext context = glXCreateContext(Window::sDisplay(), vi, sharedContext, GL_TRUE);

#elif ANDROID || EMSCRIPTEN

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
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
        };

        EGLConfig config;
        EGLint numConfigs;

        if (!eglChooseConfig(Window::sDisplay, attribs, &config, 1, &numConfigs))
            throw 0;

        EGLContext context = eglCreateContext(Window::sDisplay, config, sharedContext, contextAttribs);

#endif

        if (!sharedContext) {
            makeCurrent(window, context);

#if WINDOWS
            if (wglSwapIntervalEXT)
                wglSwapIntervalEXT(0);
#endif
        }

        return context;
    }

    void resetContext()
    {
#if WINODWS
        wglMakeCurrent(NULL, NULL);
#elif LINUX
        glXMakeCurrent(Window::sDisplay(), None, NULL);
#elif ANDROID || EMSCRIPTEN
        eglMakeCurrent(Window::sDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
#endif
    }

    void makeCurrent(Window::Window *window, ContextHandle context)
    {
#if WINDOWS
        HDC windowDC = GetDC((HWND)window->mHandle);
        if (!wglMakeCurrent(windowDC, context))
            exit(GetLastError());
#elif LINUX
        if (!glXMakeCurrent(Window::sDisplay(), window->mHandle, context))
            exit(errno);
#elif ANDROID || EMSCRIPTEN
        EGLSurface surface = (EGLSurface)window->mHandle;
        if (!eglMakeCurrent(Window::sDisplay, surface, surface, context))
            exit(errno);
#endif
    }

    void shutdownWindow(Window::Window *window, ContextHandle context)
    {
        resetContext();
#if WINODWS
        HDC device = GetDC((HWND)window->mHandle);

        ReleaseDC((HWND)window->mHandle, device);

        wglDeleteContext(context);
#elif LINUX
        glXDestroyContext(Window::sDisplay(), context);
#elif ANDROID || EMSCRIPTEN
        eglDestroyContext(Window::sDisplay, context);
#endif
    }

#if !ANDROID && !EMSCRIPTEN
    namespace {
        void OpenGLInit()
        {
            Engine::Window::WindowSettings settings;
            settings.mHidden = true;
            Window::Window *tmp = Window::sCreateWindow(settings);
            ContextHandle context = setupWindowInternal(tmp, nullptr);

#if WINDOWS
            wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));
            assert(wglCreateContextAttribsARB);
            wglSwapIntervalEXT = reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(wglGetProcAddress("wglSwapIntervalEXT"));
#endif
            bool result = gladLoadGL();
            assert(result);

            shutdownWindow(tmp, context);
            tmp->destroy();
        }
    }
#endif

    OpenGLRenderer::OpenGLRenderer(GUI::GUISystem *gui)
        : RendererComponent<OpenGLRenderer>(gui)
    {
    }

    OpenGLRenderer::~OpenGLRenderer()
    {
    }

    bool OpenGLRenderer::init()
    {
#if !ANDROID && !EMSCRIPTEN
        static std::mutex sMutex;
        static bool init = false;

        std::lock_guard guard(sMutex);
        if (!init) {
            OpenGLInit();
            init = true;
        }
#endif

        return true;
    }

    void OpenGLRenderer::finalize()
    {
    }

#if !ANDROID && !EMSCRIPTEN
    static void glDebugOutput(GLenum source,
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
            lvl = Util::ERROR_TYPE;
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            lvl = Util::WARNING_TYPE;
            break;
        case GL_DEBUG_SEVERITY_LOW:
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            lvl = Util::LOG_TYPE;
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

    std::unique_ptr<RenderWindow> OpenGLRenderer::createWindow(Window::Window *w, GUI::TopLevelWindow *topLevel, RenderWindow *sharedResources)
    {
        ContextHandle sharedContext = nullptr;
        if (sharedResources) {
            OpenGLRenderWindow *sharedWindow = dynamic_cast<OpenGLRenderWindow *>(sharedResources);
            assert(sharedWindow);
            sharedContext = sharedWindow->mContext;
        }

        ContextHandle context = setupWindowInternal(w, sharedContext);

#if !ANDROID && !EMSCRIPTEN
        glEnable(GL_DEBUG_OUTPUT);
        glCheck();
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glCheck();
        glDebugMessageCallback(glDebugOutput, nullptr);
        glCheck();
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        glCheck();
#endif

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        return std::make_unique<OpenGLRenderWindow>(w, context, topLevel);
    }
}
}

METATABLE_BEGIN(Engine::Render::OpenGLRenderer)
METATABLE_END(Engine::Render::OpenGLRenderer)

RegisterType(Engine::Render::OpenGLRenderer);