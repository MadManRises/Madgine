#include "../interfaceslib.h"

#if __linux__

#include "windowapi.h"

#include<X11/Xlib.h>
#include<GL/glx.h>

namespace Engine {
	namespace Window {

		DLL_EXPORT Display *sDisplay;

		static struct DisplayGuard {
			DisplayGuard() 
			{
				sDisplay = XOpenDisplay(NULL);
			}
				

			~DisplayGuard()
			{
				if (sDisplay)
					XCloseDisplay(sDisplay);
			}			
		} sDisplayGuard;
		
		struct LinuxWindow : Window {
			LinuxWindow(::Window hwnd) :
				Window(hwnd)
			{
			}

/*			bool handle(UINT msg, WPARAM wParam, LPARAM lParam) {
				switch (msg)
				{
				case WM_SIZE:
					onResize(LOWORD(lParam), HIWORD(lParam));
					break;
				case WM_CLOSE:
					onClose();
					break;
				case WM_DESTROY:

					return false;
					break;
				case WM_PAINT:
					onRepaint();
					break;
				}
				return true;
			}*/

			struct Geometry {
				::Window mRoot;
				int mX, mY;
				unsigned int mWidth, mHeight;
				unsigned int mBorderWidth;
				unsigned int mDepth;
			};
			Geometry getGeometry() {
				Geometry g;
				assert(XGetGeometry(sDisplay, mHandle, &g.mRoot, &g.mX, &g.mY, &g.mWidth, &g.mHeight, &g.mBorderWidth, &g.mDepth));
				return g;
			}

			virtual size_t width() override {
				return getGeometry().mWidth;
			}


			virtual size_t height() override {
				return getGeometry().mHeight;
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
				glXSwapBuffers(sDisplay, mHandle);
			}

			virtual void destroy() override {
				XDestroyWindow(sDisplay, mHandle);
			}

		};

		static std::unordered_map<::Window, LinuxWindow> sWindows;



		Window *sCreateWindow(const WindowSettings &settings) {
			assert(sDisplay);

			uintptr_t handle = settings.mHandle;
			if (!handle) {				

				static ::Window root = DefaultRootWindow(sDisplay);

				static int att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };

				static XVisualInfo *vi = glXChooseVisual(sDisplay, 0, att);				
				assert(vi);

				static Colormap cmap = XCreateColormap(sDisplay, root, vi->visual, AllocNone);

				static XSetWindowAttributes swa;

				swa.colormap = cmap;
				swa.event_mask = ExposureMask | KeyPressMask;

				handle = XCreateWindow(sDisplay, root, 0, 0, settings.mWidth, settings.mHeight, 0, vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);

				XStoreName(sDisplay, handle, settings.mTitle);

				if (!handle)
					return nullptr;
			}

			XSelectInput(sDisplay, handle, StructureNotifyMask);

			XMapWindow(sDisplay, handle);			

			XEvent event;
			do
			{
				XNextEvent(sDisplay, &event);
			} while (event.type != MapNotify);

			auto pib = sWindows.try_emplace(handle, handle);
			assert(pib.second);

			return &pib.first->second;
		}

		void sUpdate() {
			
			/*while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
			{
				
			}*/
		}

	}
}

#endif