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
#elif ANDROID
			mLastContext = eglGetCurrentContext();
			mLastWindow = Window::sFromNative((uintptr_t)eglGetCurrentSurface(EGL_DRAW));

			EGLSurface surface = (EGLSurface)window->mHandle;
			if (!eglMakeCurrent(Window::sDisplay, surface, surface, context))
				exit(errno);
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
#elif ANDROID
				if (mLastContext)
				{
					EGLSurface surface = (EGLSurface)mLastWindow->mHandle;
					if (!eglMakeCurrent(Window::sDisplay, surface, surface, mLastContext))
						exit(errno);
				}
				else
				{
					eglMakeCurrent(Window::sDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
				}
#endif
			}
		}

	}
}