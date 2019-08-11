#include "../interfaceslib.h"

#if LINUX

#include "windowapi.h"

#include <GL/glx.h>
#include <X11/Xlib.h>

namespace Engine {
namespace Window {

		DLL_EXPORT const PlatformCapabilities platformCapabilities {
        true
    };

    static Atom WM_DELETE_WINDOW;

    DLL_EXPORT Display *sDisplay()
    {

        static struct DisplayGuard {
            DisplayGuard()
            {
                display = XOpenDisplay(NULL);
                if (display)
                    WM_DELETE_WINDOW = XInternAtom(display, "WM_DELETE_WINDOW", False);
            }

            ~DisplayGuard()
            {
                if (display)
                    XCloseDisplay(display);
            }
            Display *display;
        } sDisplayGuard;

		return sDisplayGuard.display;

    }

    struct LinuxWindow : Window {
        LinuxWindow(::Window hwnd, size_t width, size_t height)
            : Window(hwnd)
            , mWidth(width)
            , mHeight(height)
        {
        }

        bool handle(const XEvent &e)
        {
            switch (e.type) {
            case ConfigureNotify: {
                const XConfigureEvent &xce = e.xconfigure;

                /* This event type is generated for a variety of
					   happenings, so check whether the window has been
					   resized. */

                if (xce.width != mWidth || xce.height != mHeight) {
                    mWidth = xce.width;
                    mHeight = xce.height;
                    onResize(mWidth, mHeight);
                }
                break;
            }
            case ClientMessage: {
                const XClientMessageEvent &xcme = e.xclient;
                if (xcme.data.l[0] == WM_DELETE_WINDOW) {
                    onClose();
                }
                break;
            }
            case DestroyNotify:
                return false;
            }
            return true;
        }

        virtual int width() override
        {
            return mWidth;
        }

        virtual int height() override
        {
            return mHeight;
        }

        virtual int renderWidth() override
        {
            //TODO
            return width();
        }

        virtual int renderHeight() override
        {
            //TODO
            return height();
        }

        virtual void beginFrame() override
        {
        }

        virtual void endFrame() override
        {
            glXSwapBuffers(sDisplay(), mHandle);
        }

		virtual int x() override
        {
            return 0;
        }

        virtual int y() override
        {
            return 0;
        }

        virtual int renderX() override
        {
            return 0;
        }

        virtual int renderY() override
        {
            return 0;
        }

        virtual void setSize(int width, int height) override
        {
            mWidth = width;
            mHeight = height;
        }

        virtual void setRenderSize(int width, int height) override
        {
            setSize(width, height);
        }

        virtual void setPos(int width, int height) override
        {
        }

        virtual void setRenderPos(int width, int height) override
        {
        }

        virtual void show() override
        {
        }

        virtual bool isMinimized() override
        {
            return false;
        }

        virtual void focus() override
        {
        }

        virtual bool hasFocus() override
        {
            return true;
        }

        virtual void setTitle(const char *title) override
        {
        }

        virtual void destroy() override
        {
            XDestroyWindow(sDisplay(), mHandle);
        }

        using Window::onClose;
        using Window::onRepaint;
        using Window::onResize;

    private:
        int mWidth;
        int mHeight;
    };

    static std::unordered_map<::Window, LinuxWindow> sWindows;

    Window *sCreateWindow(const WindowSettings &settings)
    {
        assert(sDisplay());

        uintptr_t handle = settings.mHandle;
        if (!handle) {

            static ::Window root = DefaultRootWindow(sDisplay());

            static int att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };

            static XVisualInfo *vi = glXChooseVisual(sDisplay(), 0, att);
            assert(vi);

            static Colormap cmap = XCreateColormap(sDisplay(), root, vi->visual, AllocNone);

            static XSetWindowAttributes swa;

            swa.colormap = cmap;
            swa.event_mask = ExposureMask | KeyPressMask;

            WindowSettings::WindowVector pos = settings.mPosition ? *settings.mPosition : WindowSettings::WindowVector{ 0, 0 };

            handle = XCreateWindow(sDisplay(), root, pos.x, pos.y, settings.mSize.x, settings.mSize.y, 0, vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);

            XStoreName(sDisplay(), handle, settings.mTitle);

            if (!handle)
                return nullptr;
        }

        XSelectInput(sDisplay(), handle, StructureNotifyMask);

        XMapWindow(sDisplay(), handle);

        XSetWMProtocols(sDisplay(), handle, &WM_DELETE_WINDOW, 1);

        XEvent event;
        do {
            XWindowEvent(sDisplay(), handle, StructureNotifyMask, &event);
        } while (event.type != MapNotify);

        auto pib = sWindows.try_emplace(handle, handle, settings.mSize.x, settings.mSize.y);
        assert(pib.second);

        return &pib.first->second;
    }

    void sUpdate()
    {
        XEvent event;
        while (XPending(sDisplay())) {
            XNextEvent(sDisplay(), &event);
            auto it = sWindows.find(event.xany.window);
            if (it != sWindows.end()) {
                if (!it->second.handle(event))
                    sWindows.erase(it);
            }
        }
    }

    Window *sFromNative(uintptr_t handle)
    {
        return handle ? &sWindows.at((::Window)handle) : nullptr;
    }
}
}

#endif