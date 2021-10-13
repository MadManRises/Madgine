#include "opengllib.h"
#include "Madgine/window/mainwindow.h"
#include "openglrenderwindow.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "openglrendercontext.h"
#include "openglrendertexture.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Interfaces/window/windowapi.h"

#include "openglshaderloader.h"

#if WINDOWS
#    include "wglext.h"

static PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
static PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
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

UNIQUECOMPONENT(Engine::Render::OpenGLRenderContext)

METATABLE_BEGIN(Engine::Render::OpenGLRenderContext)
METATABLE_END(Engine::Render::OpenGLRenderContext)

namespace Engine {
namespace Render {

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
            lvl = Util::MessageType::LOG_TYPE;
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            lvl = Util::MessageType::DEBUG_TYPE;
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

    ContextHandle createContext(Window::OSWindow *window, size_t samples, ContextHandle reusedContext, bool setup)
    {

#if WINDOWS
        HDC windowDC = GetDC((HWND)window->mHandle);

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

        int format;
        if (wglChoosePixelFormatARB) {
            float pfAttribFList[] = { 0, 0 };
            int piAttribIList[] = {
                WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
                WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
                WGL_COLOR_BITS_ARB, 32,
                WGL_RED_BITS_ARB, 8,
                WGL_GREEN_BITS_ARB, 8,
                WGL_BLUE_BITS_ARB, 8,
                WGL_ALPHA_BITS_ARB, 8,
                WGL_DEPTH_BITS_ARB, 24,
                WGL_STENCIL_BITS_ARB, 0,
                WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
                WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
                WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
                WGL_SAMPLES_ARB, static_cast<int>(samples),
                0, 0
            };

            UINT numFormats;
            bool result = wglChoosePixelFormatARB(windowDC, piAttribIList, pfAttribFList, 1, &format, &numFormats);
            assert(result);
            assert(numFormats > 0);
        } else {
            format = ChoosePixelFormat(windowDC, &pfd);
        }
        SetPixelFormat(windowDC, format, &pfd);

        HGLRC context;
        if (reusedContext) {
            context = reusedContext;
        } else {
            if (wglCreateContextAttribsARB) {

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
        }

        if (!reusedContext && setup) {

#if WINDOWS
            if (wglSwapIntervalEXT)
                wglSwapIntervalEXT(0);
#endif

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

            glEnable(GL_MULTISAMPLE);

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

    void destroyContext(Window::OSWindow *window, ContextHandle context, bool reusedContext)
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
                ContextHandle context = createContext(tmp, 1, nullptr, false);

#    if WINDOWS
                wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));
                assert(wglCreateContextAttribsARB);
                wglChoosePixelFormatARB = reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>(wglGetProcAddress("wglChoosePixelFormatARB"));
                assert(wglChoosePixelFormatARB);
                wglSwapIntervalEXT = reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(wglGetProcAddress("wglSwapIntervalEXT"));
#    endif
                bool result = gladLoadGL();
                assert(result);

                destroyContext(tmp, context);
                tmp->destroy();
                init = true;
            }
        }
    }
#endif

    OpenGLRenderContext::OpenGLRenderContext(Threading::TaskQueue *queue)
        : UniqueComponent(queue)
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

    std::unique_ptr<RenderTarget> OpenGLRenderContext::createRenderWindow(Window::OSWindow *w, size_t samples)
    {
        checkThread();

#if !ANDROID && !EMSCRIPTEN && !IOS
        OpenGLInit();
#endif

        OpenGLRenderWindow *sharedContext = nullptr;
        for (RenderTarget *target : mRenderTargets) {
            sharedContext = dynamic_cast<OpenGLRenderWindow *>(target);
            if (sharedContext)
                break;
        }

        std::unique_ptr<RenderTarget> window = std::make_unique<OpenGLRenderWindow>(this, w, samples, sharedContext);

        return window;
    }

}
}
