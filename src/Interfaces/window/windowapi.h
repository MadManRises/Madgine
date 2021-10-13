#pragma once

#include "windoweventlistener.h"

#include "../input/inputevents.h"

#include "windowsettings.h"

namespace Engine {
namespace Window {

    struct PlatformCapabilities {
        bool mSupportMultipleWindows;
        float mScalingFactor;
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

        virtual InterfacesVector size() = 0;

        virtual InterfacesVector renderSize() = 0;

        virtual InterfacesVector pos() = 0;

        virtual InterfacesVector renderPos() = 0;

        virtual void setSize(const InterfacesVector &size) = 0;
        virtual void setRenderSize(const InterfacesVector &size) = 0;

        virtual void setPos(const InterfacesVector &pos) = 0;
        virtual void setRenderPos(const InterfacesVector &pos) = 0;

        virtual void show() = 0;
        virtual bool isMinimized() = 0;
        virtual bool isMaximized() = 0;
        virtual bool isFullscreen() = 0;

        virtual void focus() = 0;
        virtual bool hasFocus() = 0;

        virtual void setTitle(const char *title) = 0;
        virtual std::string title() const = 0;

        virtual void destroy() = 0;

        virtual WindowData data()
        {
            return {};
        }

        //Input
        virtual bool isKeyDown(Input::Key::Key key) = 0;

        virtual void captureInput() = 0;
        virtual void releaseInput() = 0;

        const uintptr_t mHandle;

    protected:
        void onResize(const InterfacesVector &size)
        {
            for (WindowEventListener *listener : mListeners)
                listener->onResize(size);
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
            if (arg.scancode == Input::Key::Shift)
                return false;
            for (WindowEventListener *listener : mListeners)
                if (listener->injectKeyPress(arg))
                    return true;
            return false;
        }
        bool injectKeyRelease(const Input::KeyEventArgs &arg)
        {
            if (arg.scancode == Input::Key::Shift)
                return false;
            for (WindowEventListener *listener : mListeners)
                if (listener->injectKeyRelease(arg))
                    return true;
            return false;
        }
        bool injectPointerPress(const Input::PointerEventArgs &arg)
        {
            captureInput();
            for (WindowEventListener *listener : mListeners)
                if (listener->injectPointerPress(arg))
                    return true;
            return false;
        }
        bool injectPointerRelease(const Input::PointerEventArgs &arg)
        {
            releaseInput();
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
        bool injectAxisEvent(const Input::AxisEventArgs &arg)
        {
            for (WindowEventListener *listener : mListeners)
                if (listener->injectAxisEvent(arg))
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
