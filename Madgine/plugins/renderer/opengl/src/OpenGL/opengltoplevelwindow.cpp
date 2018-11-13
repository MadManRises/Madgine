#include "opengllib.h"

#include "opengltoplevelwindow.h"
#include "openglsystem.h"

#include "Madgine/gui/widgets/bar.h"
#include "Madgine/gui/widgets/button.h"
#include "Madgine/gui/widgets/checkbox.h"
#include "Madgine/gui/widgets/combobox.h"
#include "Madgine/gui/widgets/image.h"
#include "Madgine/gui/widgets/label.h"
#include "Madgine/gui/widgets/scenewindow.h"
#include "Madgine/gui/widgets/tabwidget.h"
#include "Madgine/gui/widgets/textbox.h"


#include "Interfaces/window/windowapi.h"

#include "glad.h"

namespace Engine {
	namespace GUI {

		OpenGLTopLevelWindow::OpenGLTopLevelWindow(OpenGLSystem & system) :
			TopLevelWindow(system)
		{			

			Engine::Window::WindowSettings settings;
			Window::Window *tmp = Window::sCreateWindow(settings);

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

			HDC ourWindowHandleToDeviceContext = GetDC((HWND)tmp->mHandle);

			int  letWindowsChooseThisPixelFormat;
			letWindowsChooseThisPixelFormat = ChoosePixelFormat(ourWindowHandleToDeviceContext, &pfd);
			SetPixelFormat(ourWindowHandleToDeviceContext, letWindowsChooseThisPixelFormat, &pfd);

			HGLRC ourOpenGLRenderingContext = wglCreateContext(ourWindowHandleToDeviceContext);
			wglMakeCurrent(ourWindowHandleToDeviceContext, ourOpenGLRenderingContext);

			typedef HGLRC(WINAPI * PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int *attribList);
			PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(ourOpenGLRenderingContext);
			ReleaseDC((HWND)tmp->mHandle, ourWindowHandleToDeviceContext);
			tmp->destroy();

			ourWindowHandleToDeviceContext = GetDC((HWND)window()->mHandle);

			letWindowsChooseThisPixelFormat = ChoosePixelFormat(ourWindowHandleToDeviceContext, &pfd);
			SetPixelFormat(ourWindowHandleToDeviceContext, letWindowsChooseThisPixelFormat, &pfd);
			
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126

			int attribs[] = { 
				WGL_CONTEXT_PROFILE_MASK_ARB, GL_TRUE,
				0 
			};

			ourOpenGLRenderingContext = wglCreateContextAttribsARB(ourWindowHandleToDeviceContext, NULL, attribs);
			wglMakeCurrent(ourWindowHandleToDeviceContext, ourOpenGLRenderingContext);

			typedef BOOL(WINAPI * PFNWGLSWAPINTERVALEXTPROC) (int interval);
			PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");

			wglSwapIntervalEXT(0);

			gladLoadGL();		

			//glViewport(0, 0, window()->width(), window()->height());

		}
		void OpenGLTopLevelWindow::setCustomRenderCall(void(*call)())
		{
			mCustomRenderCall = call;
		}
		bool OpenGLTopLevelWindow::update()
		{
			if (!TopLevelWindow::update())
				return false;

			window()->beginFrame();
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			if (mCustomRenderCall)
				mCustomRenderCall();
			window()->endFrame();

			return true;
		}
		void OpenGLTopLevelWindow::injectKeyPress(const Input::KeyEventArgs & arg)
		{
		}
		void OpenGLTopLevelWindow::injectKeyRelease(const Input::KeyEventArgs & arg)
		{
		}
		void OpenGLTopLevelWindow::injectMousePress(const Input::MouseEventArgs & arg)
		{
		}
		void OpenGLTopLevelWindow::injectMouseRelease(const Input::MouseEventArgs & arg)
		{
		}
		void OpenGLTopLevelWindow::injectMouseMove(const Input::MouseEventArgs & arg)
		{
		}
		bool OpenGLTopLevelWindow::isCursorVisible()
		{
			return false;
		}
		void OpenGLTopLevelWindow::setCursorVisibility(bool v)
		{
		}
		void OpenGLTopLevelWindow::setCursorPosition(const Vector2 & pos)
		{
		}
		Vector2 OpenGLTopLevelWindow::getCursorPosition()
		{
			return Vector2();
		}
		Vector3 OpenGLTopLevelWindow::getScreenSize()
		{
			return { static_cast<float>(window()->renderWidth()), static_cast<float>(window()->renderHeight()), 1 };
		}
		bool OpenGLTopLevelWindow::singleFrame()
		{
			return false;
		}
		std::unique_ptr<Widget> OpenGLTopLevelWindow::createWidget(const std::string & name)
		{
			return std::unique_ptr<Widget>();
		}
		std::unique_ptr<Bar> OpenGLTopLevelWindow::createBar(const std::string & name)
		{
			return std::unique_ptr<Bar>();
		}
		std::unique_ptr<Button> OpenGLTopLevelWindow::createButton(const std::string & name)
		{
			return std::unique_ptr<Button>();
		}
		std::unique_ptr<Checkbox> OpenGLTopLevelWindow::createCheckbox(const std::string & name)
		{
			return std::unique_ptr<Checkbox>();
		}
		std::unique_ptr<Combobox> OpenGLTopLevelWindow::createCombobox(const std::string & name)
		{
			return std::unique_ptr<Combobox>();
		}
		std::unique_ptr<Image> OpenGLTopLevelWindow::createImage(const std::string & name)
		{
			return std::unique_ptr<Image>();
		}
		std::unique_ptr<Label> OpenGLTopLevelWindow::createLabel(const std::string & name)
		{
			return std::unique_ptr<Label>();
		}
		std::unique_ptr<SceneWindow> OpenGLTopLevelWindow::createSceneWindow(const std::string & name)
		{
			return std::unique_ptr<SceneWindow>();
		}
		std::unique_ptr<TabWidget> OpenGLTopLevelWindow::createTabWidget(const std::string & name)
		{
			return std::unique_ptr<TabWidget>();
		}
		std::unique_ptr<Textbox> OpenGLTopLevelWindow::createTextbox(const std::string & name)
		{
			return std::unique_ptr<Textbox>();
		}
	}
}