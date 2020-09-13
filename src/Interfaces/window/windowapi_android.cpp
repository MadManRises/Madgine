#include "../interfaceslib.h"

#if ANDROID

#    include "windowapi.h"
#    include "windowsettings.h"

#    include <EGL/egl.h>
#    include <android/input.h>
#    include <android/native_window.h>

#    include "../threading/systemvariable.h"

#    include "../input/inputevents.h"

#    include "../util/utilmethods.h"

namespace Engine {
namespace Window {

    DLL_EXPORT const PlatformCapabilities platformCapabilities {
        false
    };

    DLL_EXPORT Threading::SystemVariable<ANativeWindow *> sNativeWindow = nullptr;

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
        AndroidWindow(EGLSurface surface)
            : OSWindow((uintptr_t)surface)
        {
            EGLint width;
            EGLint height;
            if (!eglQuerySurface(sDisplay, surface, EGL_WIDTH, &width) || !eglQuerySurface(sDisplay, surface, EGL_HEIGHT, &height))
                std::terminate();
            mSize = { width, height };
        }

        virtual void update() override
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
                        handled = handleMotionEvent(event);
                        break;
                    default:
                        LOG_ERROR("Unknown Event Type: " << AInputEvent_getType(event));
                        break;
                    }
                    AInputQueue_finishEvent(sQueue, event, handled);
                }
            }
        }

        virtual InterfacesVector size() override
        {
            return mSize;
        }

        virtual InterfacesVector renderSize() override
        {
            //TODO
            return size();
        }

        virtual void swapBuffers() override
        {
            eglSwapBuffers(sDisplay, (EGLSurface)mHandle);
        }

        virtual InterfacesVector pos() override
        {
            return { 0, 0 };
        }

        virtual InterfacesVector renderPos() override
        {
            return { 0, 0 };
        }

        virtual void setSize(const InterfacesVector &size) override
        {
            mSize = size;
        }

        virtual void setRenderSize(const InterfacesVector &size) override
        {
            setSize(size);
        }

        virtual void setPos(const InterfacesVector &pos) override
        {
        }

        virtual void setRenderPos(const InterfacesVector &pos) override
        {
        }

        virtual void show() override
        {
        }

        virtual bool isMinimized() override
        {
            return false;
        }

        virtual bool isFullscreen() override
        {
            return true;
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

        //Input
        virtual bool isKeyDown(Input::Key::Key key) override
        {
            return false;
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

        virtual void captureInput() override
        {
        }

        virtual void releaseInput() override
        {
        }

    private:
        InterfacesVector mSize;

        //Input
        InterfacesVector mLastKnownMousePos;
    };

    static std::unordered_map<EGLSurface, AndroidWindow> sWindows;

    void AndroidWindow::destroy()
    {
        eglDestroySurface(sDisplay, (EGLSurface)mHandle);
        sWindows.erase((EGLSurface)mHandle);
    }

    OSWindow *sCreateWindow(const WindowSettings &settings)
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

            sNativeWindow.wait();

            ANativeWindow_setBuffersGeometry(sNativeWindow, 0, 0, format);

            handle = eglCreateWindowSurface(sDisplay, config, sNativeWindow, 0);

            if (!handle)
                return nullptr;
        }

        auto pib = sWindows.try_emplace(handle, handle);
        assert(pib.second);

        return &pib.first->second;
    }

    OSWindow *sFromNative(uintptr_t handle)
    {
        return handle ? &sWindows.at((EGLSurface)handle) : nullptr;
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