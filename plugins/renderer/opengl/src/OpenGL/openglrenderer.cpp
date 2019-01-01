#include "opengllib.h"

#include "openglrenderer.h"

#include "glad.h"

#include "Madgine/gui/widgets/toplevelwindow.h"

#include "Interfaces/window/windowapi.h"

#include "openglrenderwindow.h"

#include "Madgine/render/camera.h"

#include <iostream>

#if __linux__
#include<X11/Xlib.h>
#include<GL/glx.h>
namespace Engine {
	namespace Window {
		extern Display *sDisplay;
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
#if _WIN32			
			wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));
			assert(wglCreateContextAttribsARB);
			wglSwapIntervalEXT = reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(wglGetProcAddress("wglSwapIntervalEXT"));
#endif
			gladLoadGL();
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

			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(glDebugOutput, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

			return std::make_unique<OpenGLRenderWindow>(w, context);
		}


		void OpenGLRenderer::shutdownWindow(Window::Window * window, ContextHandle ourOpenGLRenderingContext)
		{
#if _WIN32
			HDC ourWindowHandleToDeviceContext = GetDC((HWND)window->mHandle);

			wglMakeCurrent(NULL, NULL);

			ReleaseDC((HWND)window->mHandle, ourWindowHandleToDeviceContext);
			
			wglDeleteContext(ourOpenGLRenderingContext);
#elif __linux__
			glXMakeCurrent(Window::sDisplay, None, NULL);

			glXDestroyContext(Window::sDisplay, ourOpenGLRenderingContext);
#endif
		}

		ContextHandle OpenGLRenderer::setupWindowInternal(Window::Window * window)
		{
#if _WIN32
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

			HDC ourWindowHandleToDeviceContext = GetDC((HWND)window->mHandle);

			int  letWindowsChooseThisPixelFormat = ChoosePixelFormat(ourWindowHandleToDeviceContext, &pfd);
			SetPixelFormat(ourWindowHandleToDeviceContext, letWindowsChooseThisPixelFormat, &pfd);

			HGLRC ourOpenGLRenderingContext;
			if (wglCreateContextAttribsARB) {

#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126

				int attribs[] = {
					WGL_CONTEXT_PROFILE_MASK_ARB, GL_TRUE,
					0
				};

				ourOpenGLRenderingContext = wglCreateContextAttribsARB(ourWindowHandleToDeviceContext, NULL, attribs);
			}
			else {
				ourOpenGLRenderingContext = wglCreateContext(ourWindowHandleToDeviceContext);
			}

			wglMakeCurrent(ourWindowHandleToDeviceContext, ourOpenGLRenderingContext);

			if (wglSwapIntervalEXT)
				wglSwapIntervalEXT(0);

			return ourOpenGLRenderingContext;
#elif __linux__
			static GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };

			static XVisualInfo *vi = glXChooseVisual(Window::sDisplay, 0, att);
			assert(vi);

			GLXContext context = glXCreateContext(Window::sDisplay, vi, NULL, GL_TRUE);
			glXMakeCurrent(Window::sDisplay, window->mHandle, context);

			return context;
#endif

		}

		
	}
}


