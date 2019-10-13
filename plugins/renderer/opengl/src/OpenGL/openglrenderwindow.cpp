#include "opengllib.h"
#include "openglrenderwindow.h"
#include "Madgine/gui/widgets/toplevelwindow.h"

#include "Modules/debug/profiler/profiler.h"

#include "Modules/math/vector3.h"
#include "Modules/math/vector4.h"

#include "Madgine/gui/widgets/widget.h"

#include "Madgine/gui/vertex.h"

#include "openglrendertexture.h"

#include "openglshaderloader.h"

#include "util/openglshader.h"

#include "Interfaces/window/windowapi.h"

#include "imagedata.h"

#include "openglfontloader.h"
#include "openglmeshloader.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "Madgine/app/application.h"
#include "Madgine/scene/scenemanager.h"

RegisterType(Engine::Render::OpenGLRenderWindow)

    UNIQUECOMPONENT(Engine::Render::OpenGLRenderWindow)

        METATABLE_BEGIN(Engine::Render::OpenGLRenderWindow)
            METATABLE_END(Engine::Render::OpenGLRenderWindow)

#if WINDOWS
                typedef HGLRC(WINAPI *PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC hDC, HGLRC hShareContext, const int *attribList);
typedef BOOL(WINAPI *PFNWGLSWAPINTERVALEXTPROC)(int interval);

static PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
static PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = nullptr;

#elif LINUX
#    include <GL/glx.h>
#    include <X11/Xlib.h>
                namespace Engine
{
    namespace Window {
        extern Display *sDisplay();
    }
}
#elif ANDROID || EMSCRIPTEN
#    include <EGL/egl.h>
                namespace Engine
{
    namespace Window {
        extern EGLDisplay sDisplay;
    }
}
#endif

namespace Engine {
namespace Render {

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

    void resetContext()
    {

#if WINDOWS
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

    ContextHandle setupWindowInternal(Window::Window *window, ContextHandle reusedContext)
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

        /*if (sharedContext)
            wglShareLists(sharedContext, context);*/

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
                EGL_CONTEXT_CLIENT_VERSION, 2,
                EGL_NONE
            };

            EGLConfig config;
            EGLint numConfigs;

            if (!eglChooseConfig(Window::sDisplay, attribs, &config, 1, &numConfigs))
                throw 0;

            context = eglCreateContext(Window::sDisplay, config, /*sharedContext*/ nullptr, contextAttribs);
        }

#endif

        if (!reusedContext /* && !sharedContext*/) {
            makeCurrent(window, context);

#if WINDOWS
            if (wglSwapIntervalEXT)
                wglSwapIntervalEXT(0);
#endif
        }

        return context;
    }

    void shutdownWindow(Window::Window *window, ContextHandle context, bool reusedContext = false)
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
#endif
    }

#if !ANDROID && !EMSCRIPTEN
    namespace {
        void OpenGLInit()
        {
            static std::mutex sMutex;
            static bool init = false;

            std::lock_guard guard(sMutex);
            if (!init) {
                Engine::Window::WindowSettings settings;
                settings.mHidden = true;
                Window::Window *tmp = Window::sCreateWindow(settings);
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

    OpenGLRenderWindow::OpenGLRenderWindow(Window::Window *w, GUI::TopLevelWindow *topLevel, RenderWindow *reusedResources)
        : UniqueComponent(w)
        , mUIAtlas({ 512, 512 })
        , mTopLevelWindow(topLevel)
        , mReusedContext(reusedResources)
    {
#if !ANDROID && !EMSCRIPTEN
        OpenGLInit();
#endif

        ContextHandle reusedContext = nullptr;
        if (reusedResources) {
            OpenGLRenderWindow *reusedWindow = dynamic_cast<OpenGLRenderWindow *>(reusedResources);
            assert(reusedWindow);
            reusedContext = reusedWindow->mContext;
        }

        mContext = setupWindowInternal(w, reusedContext);

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

        glVertexAttrib2f(1, 0, 0);
        glVertexAttrib4f(2, 1, 1, 1, 1);
        glVertexAttrib2f(4, 0, 0);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        std::shared_ptr<OpenGLShader> vertexShader = OpenGLShaderLoader::load("ui_VS");
        std::shared_ptr<OpenGLShader> pixelShader = OpenGLShaderLoader::load("ui_PS");

        if (!mProgram.link(vertexShader.get(), pixelShader.get()))
            throw 0;

        mProgram.setUniform("texture", 0);

        mVAO = {};
        mVAO.bind();

        mVBO = {};
        mVBO.bind(GL_ARRAY_BUFFER);

        mVAO.enableVertexAttribute(0, &GUI::Vertex::mPos);
        mVAO.enableVertexAttribute(1, &GUI::Vertex::mColor);
        mVAO.enableVertexAttribute(2, &GUI::Vertex::mUV);

        mDefaultTexture = {};
        mDefaultTexture.setWrapMode(GL_CLAMP_TO_EDGE);
        Vector4 borderColor = { 1, 1, 1, 1 };
        mDefaultTexture.setData({ 1, 1 }, &borderColor);

        mUIAtlasTexture = {};

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
        //glDepthRange(0.0, 1.0);
    }

    OpenGLRenderWindow::~OpenGLRenderWindow()
    {
        if (!mReusedContext) {
            for (std::pair<const std::string, OpenGLFontLoader::ResourceType> &p : OpenGLFontLoader::getSingleton()) {
                p.second.unload();
            }

            for (std::pair<const std::string, OpenGLMeshLoader::ResourceType> &p : OpenGLMeshLoader::getSingleton()) {
                p.second.unload();
            }
        }

        mDefaultTexture.reset();
        mUIAtlasTexture.reset();
        mVAO.reset();
        mVBO.reset();
        shutdownWindow(window(), mContext, mReusedContext);
    }

    void OpenGLRenderWindow::render()
    {
        PROFILE();

        //TODO Remove this temp solution
        Engine::App::Application::getSingleton().getGlobalAPIComponent<Scene::SceneManager>().removeQueuedEntities();

        updateRenderTargets();

        glActiveTexture(GL_TEXTURE0);
        glCheck();
        mDefaultTexture.bind();

        Vector2 actualScreenSize { static_cast<float>(window()->renderWidth()), static_cast<float>(window()->renderHeight()) };
        Vector3 screenPos, screenSize;
        if (mTopLevelWindow) {
            std::tie(screenPos, screenSize) = mTopLevelWindow->getAvailableScreenSpace();
        } else {
            screenPos = Vector3::ZERO;
            screenSize = { actualScreenSize.x,
                actualScreenSize.y,
                1.0f };
        }

        glViewport(static_cast<GLsizei>(screenPos.x), static_cast<GLsizei>(actualScreenSize.y - screenPos.y - screenSize.y), static_cast<GLsizei>(screenSize.x), static_cast<GLsizei>(screenSize.y));

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (mTopLevelWindow) {

            mProgram.bind();

            std::map<Render::TextureDescriptor, std::vector<GUI::Vertex>> vertices;

            std::queue<std::pair<GUI::WidgetBase *, int>> q;
            for (GUI::WidgetBase *w : mTopLevelWindow->widgets()) {
                if (w->mVisible) {
                    q.push(std::make_pair(w, 0));
                }
            }
            while (!q.empty()) {

                GUI::WidgetBase *w = q.front().first;
                int depth = q.front().second;
                q.pop();

                for (GUI::WidgetBase *c : w->children()) {
                    if (c->mVisible)
                        q.push(std::make_pair(c, depth + 1));
                }

                std::vector<std::pair<std::vector<GUI::Vertex>, Render::TextureDescriptor>> localVerticesList = w->vertices(screenSize);

                Resources::ImageLoader::ResourceType *resource = w->resource();
                if (resource) {
                    auto it = mUIAtlasEntries.find(resource);
                    if (it == mUIAtlasEntries.end()) {
                        std::shared_ptr<Resources::ImageData> data = resource->loadData();
                        it = mUIAtlasEntries.try_emplace(resource, mUIAtlas.insert({ data->mWidth, data->mHeight }, [this]() { expandUIAtlas(); })).first;
                        mUIAtlasTexture.setSubData({ it->second.mArea.mTopLeft.x, it->second.mArea.mTopLeft.y }, it->second.mArea.mSize, data->mBuffer, GL_UNSIGNED_BYTE);
                    }

                    for (std::pair<std::vector<GUI::Vertex>, Render::TextureDescriptor> &localVertices : localVerticesList) {

                        std::transform(localVertices.first.begin(), localVertices.first.end(), std::back_inserter(vertices[localVertices.second]), [&](const GUI::Vertex &v) {
                            return GUI::Vertex {
                                v.mPos,
                                v.mColor,
                                { (it->second.mArea.mSize.x / (512.f * mUIAtlasSize)) * v.mUV.x + it->second.mArea.mTopLeft.x / (512.f * mUIAtlasSize),
                                    (it->second.mArea.mSize.y / (512.f * mUIAtlasSize)) * v.mUV.y + it->second.mArea.mTopLeft.y / (512.f * mUIAtlasSize) }
                            };
                        });
                    }

                } else {
                    for (std::pair<std::vector<GUI::Vertex>, Render::TextureDescriptor> &localVertices : localVerticesList) {
                        std::move(localVertices.first.begin(), localVertices.first.end(), std::back_inserter(vertices[localVertices.second]));
                    }
                }
            }

            mVAO.bind();

            for (const std::pair<const Render::TextureDescriptor, std::vector<GUI::Vertex>> &p : vertices) {
                if (!p.second.empty()) {

                    mProgram.setUniform("hasDistanceField", bool(p.first.mFlags & TextureFlag_IsDistanceField));

                    glBindTexture(GL_TEXTURE_2D, p.first.mHandle != std::numeric_limits<uint32_t>::max() ? p.first.mHandle : mUIAtlasTexture.handle());

                    mVBO.setData(GL_ARRAY_BUFFER, sizeof(p.second[0]) * p.second.size(), p.second.data());

                    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(p.second.size())); // Starting from vertex 0; 3 vertices total -> 1 triangle
                    GL_CHECK();
                }
            }
        }
    }

    void OpenGLRenderWindow::makeCurrent()
    {
        Engine::Render::makeCurrent(window(), mContext);
    }

    std::unique_ptr<RenderTarget> OpenGLRenderWindow::createRenderTarget(Scene::Camera *camera, const Vector2 &size)
    {
        return std::make_unique<OpenGLRenderTexture>(this, camera, size);
    }

    void OpenGLRenderWindow::expandUIAtlas()
    {
        if (mUIAtlasSize == 0) {
            mUIAtlasSize = 4;
            mUIAtlasTexture.setData({ mUIAtlasSize * 512, mUIAtlasSize * 512 }, nullptr, GL_UNSIGNED_BYTE);
            for (int x = 0; x < mUIAtlasSize; ++x) {
                for (int y = 0; y < mUIAtlasSize; ++y) {
                    mUIAtlas.addBin({ 512 * x, 512 * y });
                }
            }
        } else {
            /*for (int x = 0; x < mUIAtlasSize; ++x) {
                for (int y = 0; y < mUIAtlasSize; ++y) {
                    mUIAtlas.addBin({ 512 * x, 512 * (y + mUIAtlasSize) });
                    mUIAtlas.addBin({ 512 * (x + mUIAtlasSize), 512 * y });
                    mUIAtlas.addBin({ 512 * (x + mUIAtlasSize), 512 * (y + mUIAtlasSize) });
                }
            }
            mUIAtlasSize *= 2;
            mUIAtlasTexture.resize({ 512 * mUIAtlasSize, 512 * mUIAtlasSize });*/
            throw "TODO";
        }
    }

}
}