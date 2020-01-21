#include "../interfaceslib.h"

#if EMSCRIPTEN

#include "windowapi.h"

#include <EGL/egl.h>

#include <emscripten/html5.h>

#include "../threading/systemvariable.h"

namespace Engine {
namespace Window {

	DLL_EXPORT const PlatformCapabilities platformCapabilities {
        false
	};

    //DLL_EXPORT Threading::SystemVariable<ANativeWindow*> sNativeWindow = nullptr;

    DLL_EXPORT EGLDisplay sDisplay = EGL_NO_DISPLAY;

    static struct DisplayGuard {
        DisplayGuard()
        {
            sDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
            if (sDisplay != EGL_NO_DISPLAY) {
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
        EmscriptenWindow(EGLSurface surface)
            : Window((uintptr_t)surface)
        {
            EGLint width;
            EGLint height;
            if (!eglQuerySurface(sDisplay, surface, EGL_WIDTH, &width) || !eglQuerySurface(sDisplay, surface, EGL_HEIGHT, &height))
                std::terminate();
            mWidth = width;
            mHeight = height;
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

        virtual void swapBuffers() override
        {
            eglSwapBuffers(sDisplay, (EGLSurface)mHandle);
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
            emscripten_set_canvas_element_size("#canvas", mWidth, mHeight);
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

        virtual void destroy() override;

    private:
        int mWidth;
        int mHeight;
    };

    static std::unordered_map<EGLSurface, EmscriptenWindow> sWindows;

    void EmscriptenWindow::destroy()
    {
        eglDestroySurface(sDisplay, (EGLSurface)mHandle);
        sWindows.erase((EGLSurface)mHandle);
    }

    EM_BOOL eventCallback(int type, const EmscriptenUiEvent *event, void *userData)
    {
        double w;
        double h;

        emscripten_get_element_css_size(nullptr, &w, &h);

        static_cast<EmscriptenWindow *>(userData)->setSize(static_cast<int>(w), static_cast<int>(h));
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

        emscripten_set_resize_callback("#window", window, false, &eventCallback);

        double w;
        double h;

        emscripten_get_element_css_size(nullptr, &w, &h);
        window->setSize(static_cast<int>(w), static_cast<int>(h));

        return window;
    }

    void sUpdate()
    {
        /*for (std::pair<const EGLSurface, EmscriptenWindow> &window : sWindows)
			{
				window.second.resizeIfDirty();
			}*/
    }

    Window *sFromNative(uintptr_t handle)
    {
        return handle ? &sWindows.at((EGLSurface)handle) : nullptr;
    }

	std::vector<MonitorInfo> listMonitors()
    {
        double w;
        double h;

        emscripten_get_element_css_size(nullptr, &w, &h);
        
		MonitorInfo info { 0, 0, static_cast<int>(w), static_cast<int>(h) };

        return { info };
    }

}
}

#endif