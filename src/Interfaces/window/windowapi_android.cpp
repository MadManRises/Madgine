#include "../interfaceslib.h"

#if ANDROID

#include "windowapi.h"

#include <EGL/egl.h>
#include <android/native_window.h>
#include "windowapi_android.h"

namespace Engine
{
	namespace Window
	{

		DLL_EXPORT ANativeWindow *sNativeWindow = nullptr;

		void setAndroidNativeWindow(ANativeWindow * window)
		{
			sNativeWindow = window;
		}

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
			AndroidWindow(EGLSurface surface, size_t width, size_t height) :
				Window((uintptr_t)surface),
				mWidth(width),
				mHeight(height)
			{
			}

			virtual size_t width() override {			
				return mWidth;
			}

			virtual size_t height() override {
				return mHeight;
			}

			virtual size_t renderWidth() override {
				//TODO
				return width();
			}

			virtual size_t renderHeight() override {
				//TODO
				return height();
			}

			virtual void beginFrame() override {
			}

			virtual void endFrame() override {
			}

			virtual void destroy() override;

		private:
			int mWidth;
			int mHeight;
		};

		static std::unordered_map<EGLSurface, AndroidWindow> sWindows;

		void AndroidWindow::destroy(){
			eglDestroySurface(sDisplay, (EGLSurface)mHandle);
			sWindows.erase((EGLSurface)mHandle);
		}

		Window *sCreateWindow(const WindowSettings &settings)
		{
			assert(sDisplay);

			EGLSurface handle = (EGLSurface)settings.mHandle;
			if (!handle) {

				const EGLint attribs[] = {
					EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
					EGL_BLUE_SIZE, 8,
					EGL_GREEN_SIZE, 8,
					EGL_RED_SIZE, 8,
					EGL_NONE
				};

				EGLConfig config;
				EGLint numConfigs;
				EGLint format;

				if (!eglChooseConfig(sDisplay, attribs, &config, 1, &numConfigs))
					return nullptr;

				if (!eglGetConfigAttrib(sDisplay, config, EGL_NATIVE_VISUAL_ID, &format))
					return nullptr;

				while (!sNativeWindow);

				ANativeWindow_setBuffersGeometry(sNativeWindow, 0, 0, format);

				handle = eglCreateWindowSurface(sDisplay, config, sNativeWindow, 0);

				if (!handle)
					return nullptr;
			}

			auto pib = sWindows.try_emplace(handle, handle, settings.mWidth, settings.mHeight);
			assert(pib.second);

			return &pib.first->second;
		}

		void sUpdate()
		{

		}

		Window *sFromNative(uintptr_t handle) {
			return handle ? &sWindows.at((EGLSurface)handle) : nullptr;
		}


	}
}


#endif