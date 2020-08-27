#pragma once

#include "windoweventlistener.h"

namespace Engine {
namespace Window {

    struct PlatformCapabilities {
        bool mSupportMultipleWindows;
    };

    INTERFACES_EXPORT extern const PlatformCapabilities platformCapabilities;


    struct INTERFACES_EXPORT OSWindow {

        OSWindow(uintptr_t handle)
            : mHandle(handle)
        {
        }

        void addListener(WindowEventListener *listener)
        {
            mListeners.push_back(listener);
        }

        void removeListener(WindowEventListener *listener)
        {
            mListeners.erase(std::remove(mListeners.begin(), mListeners.end(), listener), mListeners.end());
        }
        
        virtual void update() = 0;

        virtual void swapBuffers() = 0;

        virtual int width() = 0;
        virtual int height() = 0;

        virtual int renderWidth() = 0;
        virtual int renderHeight() = 0;

        virtual int x() = 0;
        virtual int y() = 0;

        virtual int renderX() = 0;
        virtual int renderY() = 0;

        virtual void setSize(int width, int height) = 0;
        virtual void setRenderSize(int width, int height) = 0;

        virtual void setPos(int x, int y) = 0;
        virtual void setRenderPos(int x, int y) = 0;

        virtual void show() = 0;
        virtual bool isMinimized() = 0;

        virtual void focus() = 0;
        virtual bool hasFocus() = 0;

        virtual void setTitle(const char *title) = 0;

        virtual void destroy() = 0;


        //Input
        virtual bool isKeyDown(Input::Key::Key key) = 0;


        const uintptr_t mHandle;

    protected:
        void onResize(size_t width, size_t height)
        {
            for (WindowEventListener *listener : mListeners)
                listener->onResize(width, height);
        }

        void onClose()
        {
            std::vector<WindowEventListener *> listeners = mListeners;
            for (WindowEventListener *listener : listeners)
                listener->onClose();
        }

        void onRepaint()
        {
            for (WindowEventListener *listener : mListeners)
                listener->onRepaint();
        }

        //Input
        bool injectKeyPress(const Input::KeyEventArgs &arg)
        {
            for (WindowEventListener *listener : mListeners)
                if (listener->injectKeyPress(arg))
                    return true;
            return false;
        }
        bool injectKeyRelease(const Input::KeyEventArgs &arg)
        {
            for (WindowEventListener *listener : mListeners)
                if (listener->injectKeyRelease(arg))
                    return true;
            return false;
        }
        bool injectPointerPress(const Input::PointerEventArgs &arg)
        {
            for (WindowEventListener *listener : mListeners)
                if (listener->injectPointerPress(arg))
                    return true;
            return false;
        }
        bool injectPointerRelease(const Input::PointerEventArgs &arg)
        {
            for (WindowEventListener *listener : mListeners)
                if (listener->injectPointerRelease(arg))
                    return true;
            return false;
        }
        bool injectPointerMove(const Input::PointerEventArgs &arg)
        {
            for (WindowEventListener *listener : mListeners)
                if (listener->injectPointerMove(arg))
                    return true;
            return false;
        }

    private:
        std::vector<WindowEventListener *> mListeners;
    };

    INTERFACES_EXPORT OSWindow *sCreateWindow(const WindowSettings &settings);
    INTERFACES_EXPORT OSWindow *sFromNative(uintptr_t handle);

    struct MonitorInfo {
        int x, y;
        int width, height;
    };

    INTERFACES_EXPORT std::vector<MonitorInfo> listMonitors();

}
}