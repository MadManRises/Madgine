#include "../interfaceslib.h"

#if EMSCRIPTEN

#include "windowapi.h"

#include <EGL/egl.h>

#include <emscripten/html5.h>

#include "../threading/systemvariable.h"

namespace Engine
{
	namespace Window
	{

		//DLL_EXPORT Threading::SystemVariable<ANativeWindow*> sNativeWindow = nullptr;

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

		struct EmscriptenWindow : Window {
			EmscriptenWindow(EGLSurface surface) :
				Window((uintptr_t)surface)
			{
				EGLint width;
				EGLint height;
				if (!eglQuerySurface(sDisplay, surface, EGL_WIDTH, &width) ||
					!eglQuerySurface(sDisplay, surface, EGL_HEIGHT, &height))
					throw 0;
				mWidth = width;
				mHeight = height;
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
				eglSwapBuffers(sDisplay, (EGLSurface)mHandle);
			}

			virtual void destroy() override;

			void resize(int width, int height)
			{
				//mWidth = width;
				//mHeight = height;
				LOG(width << ", " << height);
				emscripten_set_canvas_element_size("canvas", mWidth, mHeight);
			}

		private:
			int mWidth;
			int mHeight;
			bool mDirty = false;
		};

		static std::unordered_map<EGLSurface, EmscriptenWindow> sWindows;

		void EmscriptenWindow::destroy() {
			eglDestroySurface(sDisplay, (EGLSurface)mHandle);
			sWindows.erase((EGLSurface)mHandle);
		}

		EM_BOOL eventCallback(int type, const EmscriptenUiEvent *event, void *userData)
		{
			static_cast<EmscriptenWindow*>(userData)->resize(event->windowInnerWidth, event->windowOuterWidth);
			return true;
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
					EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
					EGL_CONFORMANT, EGL_OPENGL_ES2_BIT,
					EGL_NONE
				};

				EGLConfig config;
				EGLint numConfigs;
				EGLint format;

				if (!eglChooseConfig(sDisplay, attribs, &config, 1, &numConfigs))
					return nullptr;
				
				if (!eglGetConfigAttrib(sDisplay, config, EGL_NATIVE_VISUAL_ID, &format))
					return nullptr;
				
				//sNativeWindow.wait();

				//ANativeWindow_setBuffersGeometry(sNativeWindow, 0, 0, format);

				handle = eglCreateWindowSurface(sDisplay, config, 0, 0);
				if (!handle)
					return nullptr;
			}

			auto pib = sWindows.try_emplace(handle, handle);
			assert(pib.second);

			EmscriptenWindow *window = &pib.first->second;

			emscripten_set_resize_callback(nullptr, window, false, &eventCallback);

			return window;
		}

		void sUpdate()
		{
			/*for (std::pair<const EGLSurface, EmscriptenWindow> &window : sWindows)
			{
				window.second.resizeIfDirty();
			}*/
		}

		Window *sFromNative(uintptr_t handle) {
			return handle ? &sWindows.at((EGLSurface)handle) : nullptr;
		}


	}
}


#endif