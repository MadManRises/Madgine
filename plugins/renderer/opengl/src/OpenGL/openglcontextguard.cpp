#include "opengllib.h"

#include "openglcontextguard.h"

#include "Interfaces/window/windowapi.h"


#if LINUX
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


		OpenGLContextGuard::OpenGLContextGuard(Window::Window *window, ContextHandle context)			
		{
#if WINDOWS
			mLastContext = wglGetCurrentContext();
			mLastWindow = Window::sFromNative((uintptr_t)WindowFromDC(wglGetCurrentDC()));

			HDC ourWindowHandleToDeviceContext = GetDC((HWND)window->mHandle);
			if (!wglMakeCurrent(ourWindowHandleToDeviceContext, context))
				exit(GetLastError());
#elif LINUX
			throw 0;
			//TODO so

			glXMakeCurrent(Window::sDisplay, window->mHandle, context);
#endif
		}

		OpenGLContextGuard::OpenGLContextGuard(OpenGLContextGuard&& other) :
			mLastContext(other.mLastContext),
			mLastWindow(other.mLastWindow)
		{
			assert(!other.mMovedFrom);
			other.mMovedFrom = true;
		}

		OpenGLContextGuard::~OpenGLContextGuard()
		{
			if (!mMovedFrom)
			{
#if WINDOWS
				if (mLastContext) {
					HDC ourWindowHandleToDeviceContext = GetDC((HWND)mLastWindow->mHandle);
					if (!wglMakeCurrent(ourWindowHandleToDeviceContext, mLastContext))
						exit(GetLastError());
				}
				else
				{
					wglMakeCurrent(NULL, NULL);
				}
#elif LINUX
				throw 0;
				//TODO
				glXMakeCurrent(Window::sDisplay, None, NULL);
#endif
			}
		}

	}
}