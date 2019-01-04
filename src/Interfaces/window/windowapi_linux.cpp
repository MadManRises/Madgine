#include "../interfaceslib.h"

#if !__ANDROID__ && __linux__

#include "windowapi.h"

#include<X11/Xlib.h>
#include<GL/glx.h>

namespace Engine {
	namespace Window {

		DLL_EXPORT Display *sDisplay;

		static Atom WM_DELETE_WINDOW;

		static struct DisplayGuard {
			DisplayGuard() 
			{
				sDisplay = XOpenDisplay(NULL);
				if (sDisplay)
					WM_DELETE_WINDOW = XInternAtom(sDisplay, "WM_DELETE_WINDOW", False);
			}
				

			~DisplayGuard()
			{
				if (sDisplay)
					XCloseDisplay(sDisplay);
			}			
		} sDisplayGuard;
		
		struct LinuxWindow : Window {
			LinuxWindow(::Window hwnd, size_t width, size_t height) :
				Window(hwnd),
				mWidth(width),
				mHeight(height)
			{
			}

			bool handle(const XEvent &e) {
				switch (e.type)
				{
				case ConfigureNotify:
				{
					const XConfigureEvent &xce = e.xconfigure;

					/* This event type is generated for a variety of
					   happenings, so check whether the window has been
					   resized. */

					if (xce.width != mWidth ||
						xce.height != mHeight) {
						mWidth = xce.width;
						mHeight = xce.height;
						onResize(mWidth, mHeight);
					}
					break;
				}
				case ClientMessage:
				{
					const XClientMessageEvent &xcme = e.xclient;
					if (xcme.data.l[0] == WM_DELETE_WINDOW)
					{
						onClose();
					}
					break;
				}
				case DestroyNotify:
					return false;
				default:
					LOG(Database::message("Unknown message type: ", "")(e.type));
				}
				return true;
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
				glXSwapBuffers(sDisplay, mHandle);
			}

			virtual void destroy() override {
				XDestroyWindow(sDisplay, mHandle);
			}

			using Window::onResize;
			using Window::onClose;
			using Window::onRepaint;

		private:
			int mWidth;
			int mHeight;
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

			
			XSetWMProtocols(sDisplay, handle, &WM_DELETE_WINDOW, 1);

			XEvent event;
			do
			{
				XWindowEvent(sDisplay, handle, StructureNotifyMask, &event);
			} while (event.type != MapNotify);

			auto pib = sWindows.try_emplace(handle, handle, settings.mWidth, settings.mHeight);
			assert(pib.second);

			return &pib.first->second;
		}

		void sUpdate() {
			XEvent event;
			while (XPending(sDisplay))
			{
				XNextEvent(sDisplay, &event);
				auto it = sWindows.find(event.xany.window);
				if (it != sWindows.end()) {
					if (!it->second.handle(event))
						sWindows.erase(it);
				}
			}
		}

	}
}

#endif