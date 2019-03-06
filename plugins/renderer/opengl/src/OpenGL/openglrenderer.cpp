#include "opengllib.h"

#include "openglrenderer.h"

#include "glad.h"

#include "client/gui/widgets/toplevelwindow.h"

#include "Interfaces/window/windowapi.h"

#include "openglrenderwindow.h"

#include "client/render/camera.h"

#include <iostream>

#include "openglcontextguard.h"

#if LINUX
#include<X11/Xlib.h>
#include<GL/glx.h>
namespace Engine {
	namespace Window {
		extern Display *sDisplay;
	}
}
#elif ANDROID
#include<EGL/egl.h>
namespace Engine {
	namespace Window {
		extern EGLDisplay sDisplay;
	}
}
#endif

namespace Engine {
	
	namespace Render {

		OpenGLRenderer::OpenGLRenderer(GUI::GUISystem *gui) :
			RendererComponent<OpenGLRenderer>(gui)
#if _WIN32
			, wglCreateContextAttribsARB(nullptr),
			wglSwapIntervalEXT(nullptr)
#endif
		{			
		}

		OpenGLRenderer::~OpenGLRenderer()
		{
		}

		bool OpenGLRenderer::init()
		{	
			Engine::Window::WindowSettings settings;
			Window::Window *tmp = Window::sCreateWindow(settings);
			ContextHandle context = setupWindowInternal(tmp);
			{
				OpenGLContextGuard guard(tmp, context);
#if WINDOWS			
				wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));
				assert(wglCreateContextAttribsARB);
				wglSwapIntervalEXT = reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(wglGetProcAddress("wglSwapIntervalEXT"));
#endif
#if ANDROID
				assert(gladLoadGLLoader((GLADloadproc)&eglGetProcAddress));
#else
				assert(gladLoadGL());
#endif
			}
			shutdownWindow(tmp, context);
			tmp->destroy();
			return true;
		}

		void OpenGLRenderer::finalize()
		{

		}

		static void APIENTRY glDebugOutput(GLenum source,
			GLenum type,
			GLuint id,
			GLenum severity,
			GLsizei length,
			const GLchar *message,
			const void *userParam)
		{
			// ignore non-significant error/warning codes
			if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

			std::cout << "---------------" << std::endl;
			std::cout << "Debug message (" << id << "): " << message << std::endl;

			switch (source)
			{
			case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
			case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
			case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
			case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
			case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
			} std::cout << std::endl;

			switch (type)
			{
			case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
			case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
			case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
			case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
			case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
			case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
			case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
			} std::cout << std::endl;

			switch (severity)
			{
			case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
			case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
			case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
			case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
			} std::cout << std::endl;
			std::cout << std::endl;
		}

		std::unique_ptr<RenderWindow> OpenGLRenderer::createWindow(GUI::TopLevelWindow * w)
		{
			ContextHandle context = setupWindowInternal(w->window());

			OpenGLContextGuard guard(w->window(), context);

			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			if (glDebugMessageCallback && glDebugMessageControl)
			{
				glDebugMessageCallback(glDebugOutput, nullptr);
				glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
			}

			return std::make_unique<OpenGLRenderWindow>(w, context);
		}


		void OpenGLRenderer::shutdownWindow(Window::Window * window, ContextHandle ourOpenGLRenderingContext)
		{
#if WINODWS
			HDC ourWindowHandleToDeviceContext = GetDC((HWND)window->mHandle);

			ReleaseDC((HWND)window->mHandle, ourWindowHandleToDeviceContext);
			
			wglDeleteContext(ourOpenGLRenderingContext);
#elif LINUX

			glXDestroyContext(Window::sDisplay, ourOpenGLRenderingContext);
#endif
		}


		ContextHandle OpenGLRenderer::setupWindowInternal(Window::Window * window)
		{
#if WINDOWS
			PIXELFORMATDESCRIPTOR pfd =
			{
				sizeof(PIXELFORMATDESCRIPTOR),
				1,
				PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
				PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
				32,                   // Colordepth of the framebuffer.
				0, 0, 0, 0, 0, 0,
				0,
				0,
				0,
				0, 0, 0, 0,
				24,                   // Number of bits for the depthbuffer
				8,                    // Number of bits for the stencilbuffer
				0,                    // Number of Aux buffers in the framebuffer.
				PFD_MAIN_PLANE,
				0,
				0, 0, 0
			};

			HDC windowDC = GetDC((HWND)window->mHandle);

			int  format = ChoosePixelFormat(windowDC, &pfd);
			SetPixelFormat(windowDC, format, &pfd);

			HGLRC context;
			if (wglCreateContextAttribsARB) {

#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126

				int attribs[] = {
					WGL_CONTEXT_PROFILE_MASK_ARB, GL_TRUE,
					0
				};

				context = wglCreateContextAttribsARB(windowDC, NULL, attribs);
			}
			else {
				context = wglCreateContext(windowDC);
			}

			OpenGLContextGuard guard(window, context);

			if (wglSwapIntervalEXT)
				wglSwapIntervalEXT(0);

			return context;
#elif LINUX
			static GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };

			static XVisualInfo *vi = glXChooseVisual(Window::sDisplay, 0, att);
			assert(vi);

			GLXContext context = glXCreateContext(Window::sDisplay, vi, NULL, GL_TRUE);

			return context;
#elif ANDROID

			const EGLint attribs[] = {
					EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
					EGL_BLUE_SIZE, 8,
					EGL_GREEN_SIZE, 8,
					EGL_RED_SIZE, 8,
					EGL_NONE
			};

			EGLConfig config;
			EGLint numConfigs;

			if (!eglChooseConfig(Window::sDisplay, attribs, &config, 1, &numConfigs))
				throw 0;

			EGLContext context = eglCreateContext(Window::sDisplay, config, 0, 0);

			return context;
#endif

		}

		
	}
}


