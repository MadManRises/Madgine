#include "../interfaceslib.h"

#if ANDROID

#    include "windowapi.h"
#    include "windowsettings.h"

#    include <EGL/egl.h>
#    include <android/input.h>
#    include <android/native_window.h>

#    include "Generic/systemvariable.h"

#    include "../input/inputevents.h"

#    include "../util/utilmethods.h"

namespace Engine {
namespace Window {

    DLL_EXPORT const PlatformCapabilities platformCapabilities {
        false,
        3.0f
    };

    DLL_EXPORT SystemVariable<ANativeWindow *> sNativeWindow = nullptr;

    DLL_EXPORT EGLDisplay sDisplay = EGL_NO_DISPLAY;

    DLL_EXPORT AInputQueue *sQueue = nullptr;

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

    struct AndroidWindow final : OSWindow {
        AndroidWindow(EGLSurface surface, WindowEventListener *listener)
            : OSWindow((uintptr_t)surface, listener)
        {
            EGLint width;
            EGLint height;
            if (!eglQuerySurface(sDisplay, surface, EGL_WIDTH, &width) || !eglQuerySurface(sDisplay, surface, EGL_HEIGHT, &height))
                std::terminate();
            mSize = { width, height };
        }

        bool handleMotionEvent(const AInputEvent *event)
        {
            int32_t action = AMotionEvent_getAction(event);
            size_t pointer_index = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
            InterfacesVector position {
                static_cast<int>(AMotionEvent_getX(event, pointer_index)),
                static_cast<int>(AMotionEvent_getY(event, pointer_index))
            };

            bool handled = false;

            switch (action & AMOTION_EVENT_ACTION_MASK) {
            case AMOTION_EVENT_ACTION_DOWN:
                handled = injectPointerMove({ position, position,
                    position - mLastKnownMousePos });
                handled |= injectPointerPress({ position, position,
                    Input::MouseButton::LEFT_BUTTON });
                break;
            case AMOTION_EVENT_ACTION_UP:
                handled = injectPointerRelease({ position, position,
                    Input::MouseButton::LEFT_BUTTON });
                break;
            case AMOTION_EVENT_ACTION_MOVE:
                handled = injectPointerMove({ position, position,
                    position - mLastKnownMousePos });
                break;
            case AMOTION_EVENT_ACTION_CANCEL:
                LOG("Motion Cancel");
                break;
            case AMOTION_EVENT_ACTION_OUTSIDE:
                LOG("Motion Outside");
                break;
            case AMOTION_EVENT_ACTION_POINTER_DOWN:
                LOG("Motion Pointer Down");
                break;
            case AMOTION_EVENT_ACTION_POINTER_UP:
                LOG("Motion Pointer Up");
                break;
            case AMOTION_EVENT_ACTION_HOVER_MOVE:
                LOG("Motion Hover Move");
                break;
            case AMOTION_EVENT_ACTION_SCROLL:
                LOG("Motion Scroll");
                break;
            case AMOTION_EVENT_ACTION_HOVER_ENTER:
                LOG("Motion Hover Enter");
                break;
            case AMOTION_EVENT_ACTION_HOVER_EXIT:
                LOG("Motion Hover Exit");
                break;
            default:
                LOG_ERROR("Unknown Motion Event Type: " << (action & AMOTION_EVENT_ACTION_MASK));
                break;
            }

            mLastKnownMousePos = position;
            return handled;
        }

        InterfacesVector mSize;

        //Input
        InterfacesVector mLastKnownMousePos;
    };

    static std::unordered_map<EGLSurface, AndroidWindow>
        sWindows;

    void OSWindow::update()
    {
        if (sQueue) {
            AInputEvent *event = NULL;
            while (AInputQueue_getEvent(sQueue, &event) >= 0) {
                if (AInputQueue_preDispatchEvent(sQueue, event)) {
                    continue;
                }
                bool handled = false;
                switch (AInputEvent_getType(event)) {
                case AINPUT_EVENT_TYPE_KEY:
                    //TODO
                    std::terminate();
                    break;
                case AINPUT_EVENT_TYPE_MOTION:
                    handled = static_cast<AndroidWindow *>(this)->handleMotionEvent(event);
                    break;
                default:
                    LOG_ERROR("Unknown Event Type: " << AInputEvent_getType(event));
                    break;
                }
                AInputQueue_finishEvent(sQueue, event, handled);
            }
        }
    }

    InterfacesVector OSWindow::size()
    {
        return static_cast<AndroidWindow *>(this)->mSize;
    }

    InterfacesVector OSWindow::renderSize()
    {
        //TODO
        return size();
    }

    InterfacesVector OSWindow::pos()
    {
        return { 0, 0 };
    }

    InterfacesVector OSWindow::renderPos()
    {
        return { 0, 0 };
    }

    void OSWindow::setSize(const InterfacesVector &size)
    {
        static_cast<AndroidWindow *>(this)->mSize = size;
    }

    void OSWindow::setRenderSize(const InterfacesVector &size)
    {
        setSize(size);
    }

    void OSWindow::setPos(const InterfacesVector &pos)
    {
    }

    void OSWindow::setRenderPos(const InterfacesVector &pos)
    {
    }

    void OSWindow::show()
    {
    }

    bool OSWindow::isMinimized()
    {
        return false;
    }

    bool OSWindow::isMaximized()
    {
        return false;
    }

    bool OSWindow::isFullscreen()
    {
        return true;
    }

    void OSWindow::focus()
    {
    }

    bool OSWindow::hasFocus()
    {
        return true;
    }

    void OSWindow::setTitle(const char *title)
    {
    }

    std::string OSWindow::title() const
    {
        return "";
    }

    void OSWindow::close()
    {
    }

    void OSWindow::destroy()
    {
        eglDestroySurface(sDisplay, (EGLSurface)mHandle);
        sWindows.erase((EGLSurface)mHandle);
    }

    void OSWindow::setCursorIcon(Input::CursorIcon icon)
    {
        /*SetCursor(LoadCursor(NULL, [](Input::CursorIcon icon) {
                switch (icon) {
                case Input::CursorIcon::Arrow:
                    return IDC_ARROW;
                case Input::CursorIcon::TextInput:
                    return IDC_IBEAM;
                case Input::CursorIcon::ResizeAll:
                    return IDC_SIZEALL;
                case Input::CursorIcon::ResizeNS:
                    return IDC_SIZENS;
                case Input::CursorIcon::ResizeEW:
                    return IDC_SIZEWE;
                case Input::CursorIcon::ResizeNESW:
                    return IDC_SIZENESW;
                case Input::CursorIcon::ResizeNWSE:
                    return IDC_SIZENWSE;
                case Input::CursorIcon::Hand:
                    return IDC_HAND;
                case Input::CursorIcon::NotAllowed:
                    return IDC_NO;
                default:
                    throw 0;
                }
            }(icon)));*/
    }

    std::string OSWindow::getClipboardString()
    {
        return "";
    }

    bool OSWindow::setClipboardString(std::string_view s)
    {
        return true;
    }

    WindowData OSWindow::data()
    {
        return {};
    }

    //Input
    bool OSWindow::isKeyDown(Input::Key::Key key)
    {
        return false;
    }

    void OSWindow::captureInput()
    {
    }

    void OSWindow::releaseInput()
    {
    }

    OSWindow *sCreateWindow(const WindowSettings &settings, WindowEventListener *listener)
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
                /* EGL_SAMPLE_BUFFERS, 1,*/
                EGL_NONE
            };

            EGLConfig config;
            EGLint numConfigs;
            EGLint format;

            if (!eglChooseConfig(sDisplay, attribs, &config, 1, &numConfigs))
                return nullptr;

            if (!eglGetConfigAttrib(sDisplay, config, EGL_NATIVE_VISUAL_ID, &format))
                return nullptr;

            sNativeWindow.wait();

            ANativeWindow_setBuffersGeometry(sNativeWindow, 0, 0, format);

            handle = eglCreateWindowSurface(sDisplay, config, sNativeWindow, 0);

            if (!handle)
                return nullptr;
        }

        auto pib = sWindows.try_emplace(handle, handle, listener);
        assert(pib.second);

        return &pib.first->second;
    }

    std::vector<MonitorInfo> listMonitors()
    {
        int width = ANativeWindow_getWidth(sNativeWindow);
        int height = ANativeWindow_getHeight(sNativeWindow);

        MonitorInfo info { 0, 0, width, height };

        return { info };
    }

}
}

#endif
