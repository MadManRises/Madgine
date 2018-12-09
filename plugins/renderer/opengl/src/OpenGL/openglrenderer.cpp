#include "opengllib.h"

#include "openglrenderer.h"

#include "glad.h"

#include "Madgine/gui/widgets/toplevelwindow.h"

#include "Interfaces/window/windowapi.h"

#include "openglrenderwindow.h"

#include "Madgine/render/camera.h"

namespace Engine {
	namespace Render {

		OpenGLRenderer::OpenGLRenderer(GUI::GUISystem *gui) :
			Renderer<OpenGLRenderer>(gui),
			wglCreateContextAttribsARB(nullptr),
			wglSwapIntervalEXT(nullptr)
		{			
		}

		OpenGLRenderer::~OpenGLRenderer()
		{
		}

		bool OpenGLRenderer::init()
		{		
			Engine::Window::WindowSettings settings;
			Window::Window *tmp = Window::sCreateWindow(settings);
			HGLRC context = setupWindowInternal(tmp);
			wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));
			assert(wglCreateContextAttribsARB);
			wglSwapIntervalEXT = reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(wglGetProcAddress("wglSwapIntervalEXT"));
			gladLoadGL();
			shutdownWindow(tmp, context);
			tmp->destroy();
			return true;
		}

		void OpenGLRenderer::finalize()
		{

		}

		std::unique_ptr<RenderWindow> OpenGLRenderer::createWindow(GUI::TopLevelWindow * w)
		{
			
			HGLRC context = setupWindowInternal(w->window());

			return std::make_unique<OpenGLRenderWindow>(w, context);
		}


		void OpenGLRenderer::shutdownWindow(Window::Window * window, HGLRC ourOpenGLRenderingContext)
		{
			HDC ourWindowHandleToDeviceContext = GetDC((HWND)window->mHandle);

			wglMakeCurrent(NULL, NULL);

			ReleaseDC((HWND)window->mHandle, ourWindowHandleToDeviceContext);
			
			wglDeleteContext(ourOpenGLRenderingContext);

		}

		HGLRC OpenGLRenderer::setupWindowInternal(Window::Window * window)
		{
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
		}

		
	}
}


