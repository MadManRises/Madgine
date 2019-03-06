#include "../interfaceslib.h"

#if ANDROID

#include "windowapi.h"

#include <EGL/egl.h>

namespace Engine
{
	namespace Window
	{

		DLL_EXPORT EGLDisplay sDisplay = EGL_NO_DISPLAY;

		static struct DisplayGuard {
			DisplayGuard()
			{
				sDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
				if (sDisplay != EGL_NO_DISPLAY)
				{
					if (!eglInitialize(sDisplay, nullptr, nullptr))
						sDisplay = EGL_NO_DISPLAY;
				}
			}


			~DisplayGuard()
			{
				if (sDisplay != EGL_NO_DISPLAY)
					eglTerminate(sDisplay);
			}
		} sDisplayGuard;

		struct AndroidWindow : Window {

		};

		Window *sCreateWindow(const WindowSettings &settings)
		{

		}

	}
}


#endif