#include "opengllib.h"
#include "Madgine/window/mainwindow.h"
#include "openglrenderwindow.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "openglrendercontext.h"
#include "openglrendertexture.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Interfaces/window/windowapi.h"

#include "openglshaderloader.h"

#include "openglmeshloader.h"
#include "openglpipelineloader.h"
#include "opengltextureloader.h"

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

#    define X(VAL)    \
        {             \
            VAL, #VAL \
        }
static constexpr struct {
    EGLint attribute;
    const char *name;
} eglAttributeNames[] = {
    X(EGL_BUFFER_SIZE),
    X(EGL_RED_SIZE),
    X(EGL_GREEN_SIZE),
    X(EGL_BLUE_SIZE),
    X(EGL_ALPHA_SIZE),
    X(EGL_CONFIG_CAVEAT),
    X(EGL_CONFIG_ID),
    X(EGL_DEPTH_SIZE),
    X(EGL_LEVEL),
    X(EGL_MAX_PBUFFER_WIDTH),
    X(EGL_MAX_PBUFFER_HEIGHT),
    X(EGL_MAX_PBUFFER_PIXELS),
    X(EGL_NATIVE_RENDERABLE),
    X(EGL_NATIVE_VISUAL_ID),
    X(EGL_NATIVE_VISUAL_TYPE),
    //X(EGL_PRESERVED_RESOURCES),
    X(EGL_SAMPLE_BUFFERS),
    X(EGL_SAMPLES),
    // X(EGL_STENCIL_BITS),
    X(EGL_SURFACE_TYPE),
    X(EGL_TRANSPARENT_TYPE),
    // X(EGL_TRANSPARENT_RED),
    // X(EGL_TRANSPARENT_GREEN),
    // X(EGL_TRANSPARENT_BLUE)
};
#    undef X
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
            lvl = Util::MessageType::INFO_TYPE;
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
#elif IOS
        IOSBridge::resetContext();
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
            LOG_ERROR("Error-Code: " << eglGetError());
            std::terminate();
        }
#elif OSX
        OSXBridge::makeCurrent(context);
#elif IOS
        IOSBridge::makeCurrent(context);
#endif
    }

    void swapBuffers(Window::OSWindow *window, ContextHandle context)
    {
#if WINDOWS
        SwapBuffers(GetDC((HWND)window->mHandle));
#elif LINUX
        glXSwapBuffers(Window::sDisplay(), window->mHandle);
#elif ANDROID || EMSCRIPTEN
        eglSwapBuffers(Window::sDisplay, (EGLSurface)window->mHandle);
#elif OSX
        OSXBridge::swapBuffers(context);
#elif IOS
        IOSBridge::swapBuffers(context);
#else
#    error "Unsupported Platform!"
#endif
    }

    static bool checkMultisampling()
    {
#if MULTISAMPLING
#    if ANDROID || EMSCRIPTEN
        const EGLint attribs[] = {
            EGL_SAMPLE_BUFFERS, 1,
            EGL_NONE
        };

        EGLint numConfigs;

        return eglChooseConfig(Window::sDisplay, attribs, nullptr, 0, &numConfigs);
#    else
        return glTexImage2DMultisample;
#    endif
#else
        return false;
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
                /* EGL_SAMPLE_BUFFERS, 1,
                EGL_SAMPLES, static_cast<EGLint>(samples),*/
                EGL_NONE
            };

            const EGLint contextAttribs[] = {
                EGL_CONTEXT_CLIENT_VERSION, 3,
                EGL_NONE
            };

            EGLConfig config;
            EGLint numConfigs;

            if (!eglChooseConfig(Window::sDisplay, attribs, &config, 1, &numConfigs)) {
                LOG_ERROR("Could not find suitable EGL configuration");

                bool result = eglGetConfigs(Window::sDisplay, NULL, 0, &numConfigs);
                assert(result);

                LOG("Number of EGL configuration: " << numConfigs);

                std::unique_ptr<EGLConfig[]> configs = std::make_unique<EGLConfig[]>(numConfigs);

                result = eglGetConfigs(Window::sDisplay, configs.get(), numConfigs, &numConfigs);
                assert(result);

                for (int i = 0; i < numConfigs; i++) {
                    Util::LogDummy out { Engine::Util::MessageType::INFO_TYPE };
                    out << "Configuration:\n";
                    EGLConfig config = configs[i];
                    for (int j = 0; j < sizeof(eglAttributeNames) / sizeof(eglAttributeNames[0]); j++) {
                        EGLint value = -1;
                        result = eglGetConfigAttrib(Window::sDisplay, config, eglAttributeNames[j].attribute, &value);
                        if (result) {
                            out << " " << eglAttributeNames[j].name << " = " << value << "\n";
                        }
                    }
                }
                std::terminate();
            }

            context = eglCreateContext(Window::sDisplay, config, /*sharedContext*/ nullptr, contextAttribs);
            if (!context) {
                LOG_ERROR("eglCreateContext - ErrorCode: " << eglGetError());
            }
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

            //Pos
            glVertexAttrib3f(0, 0, 0, 0);
            GL_CHECK();
            //Pos2
            glVertexAttrib2f(1, 0, 0);
            GL_CHECK();
            //Color
            glVertexAttrib4f(3, 1, 1, 1, 1);
            GL_CHECK();
            //UV
            glVertexAttrib2f(4, 0, 0);
            GL_CHECK();
            //BoneIndices
            glVertexAttribI4i(5, 0, 0, 0, 0);
            GL_CHECK();
            //Weights
            glVertexAttrib4f(6, 0, 0, 0, 0);
            GL_CHECK();

            glEnable(GL_BLEND);
            GL_CHECK();
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            GL_CHECK();

            glEnable(GL_DEPTH_TEST);
            GL_CHECK();
            glDepthMask(GL_TRUE);
            GL_CHECK();
            glDepthFunc(GL_LESS);
            GL_CHECK();
            //glDepthRange(0.0, 1.0);

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
#if !OSX
                GL_LOG_PROPERTY(GL_EXTENSIONS);
#endif
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
            static std::once_flag once;

            std::call_once(once, []() {
                Engine::Window::WindowSettings settings;
                settings.mHidden = true;
                Window::OSWindow *tmp = Window::sCreateWindow(settings, nullptr);
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
            });
        }
    }
#endif

    OpenGLRenderContext::OpenGLRenderContext(Threading::TaskQueue *queue)
        : Component(queue)
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

    void OpenGLRenderContext::unloadAllResources()
    {
        RenderContext::unloadAllResources();

        for (std::pair<const std::string, OpenGLPipelineLoader::Resource> &res : OpenGLPipelineLoader::getSingleton()) {
            res.second.forceUnload();
        }

        for (std::pair<const std::string, OpenGLTextureLoader::Resource> &res : OpenGLTextureLoader::getSingleton()) {
            res.second.forceUnload();
        }

        for (std::pair<const std::string, OpenGLMeshLoader::Resource> &res : OpenGLMeshLoader::getSingleton()) {
            res.second.forceUnload();
        }
    }

    bool OpenGLRenderContext::supportsMultisampling() const
    {
        return checkMultisampling();
    }

    static constexpr GLenum vTypes[] = {
        GL_FLOAT,
        GL_FLOAT,
        GL_FLOAT,
        GL_FLOAT,
        GL_FLOAT,
        GL_FLOAT,
        GL_INT,
        GL_FLOAT
    };

    void OpenGLRenderContext::bindFormat(VertexFormat format)
    {
        assert(!format.has(0) || !format.has(1));

#if !OPENGL_ES || OPENGL_ES >= 310
#    if !OPENGL_ES
        if (glVertexAttribFormat) {
#    endif
            auto pib = mVAOs.try_emplace(format, create);
            pib.first->second.bind();
            if (!pib.second)
                return;

            GLuint offset = 0;
            for (size_t i = 0; i < VertexElements::size; ++i) {
                size_t attribIndex = i > 0 ? i - 1 : 0;
                if (format.has(i)) {
                    if (vTypes[i] == GL_FLOAT)
                        glVertexAttribFormat(attribIndex, sVertexElementSizes[i] / 4, vTypes[i], GL_FALSE, offset);
                    else
                        glVertexAttribIFormat(attribIndex, sVertexElementSizes[i] / 4, vTypes[i], offset);
                    GL_CHECK();
                    glVertexAttribBinding(attribIndex, 0);
                    GL_CHECK();
                    glEnableVertexAttribArray(attribIndex);
                    offset += sVertexElementSizes[i];
                } else {
                    //glDisableVertexAttribArray(attribIndex);
                }
            }

            return;
#    if !OPENGL_ES
        }
#    endif
#endif

        GLuint stride = 0;
        for (size_t i = 0; i < VertexElements::size; ++i) {
            if (format.has(i))
                stride += sVertexElementSizes[i];
        }

        const std::byte *offset = 0;
        for (size_t i = 0; i < VertexElements::size; ++i) {
            size_t attribIndex = i > 0 ? i - 1 : 0;
            if (format.has(i)) {
                if (vTypes[i] == GL_FLOAT)
                    glVertexAttribPointer(attribIndex, sVertexElementSizes[i] / 4, vTypes[i], GL_FALSE, stride, offset);
                else
                    glVertexAttribIPointer(attribIndex, sVertexElementSizes[i] / 4, vTypes[i], stride, offset);
                GL_CHECK();
                glEnableVertexAttribArray(attribIndex);
                offset += sVertexElementSizes[i];
            } else {
                glDisableVertexAttribArray(attribIndex);
            }
        }
        if (format.has(0) || format.has(1))
            glEnableVertexAttribArray(0);
    }

    void OpenGLRenderContext::unbindFormat()
    {
#if !OPENGL_ES || OPENGL_ES >= 310
        glBindVertexArray(0);
        GL_CHECK();
#endif
    }

}
}
