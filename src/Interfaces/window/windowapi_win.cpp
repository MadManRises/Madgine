#include "../interfaceslib.h"

#if WINDOWS

#    include "windowapi.h"
#    include "windoweventlistener.h"
#    include "windowsettings.h"

#    include "../input/inputevents.h"
#    include "../input/rawinput_win.h"

#    define NOMINMAX
#    include <Windows.h>
#    include <windowsx.h>

namespace Engine {
namespace Window {

    DLL_EXPORT const PlatformCapabilities platformCapabilities {
        true,
        1.0f
    };

    struct WindowsWindow final : OSWindow {
        WindowsWindow(HWND hwnd)
            : OSWindow((uintptr_t)hwnd)
            , mKeyDown {}
        {
        }

        virtual void update() override
        {
            MSG msg;
            while (PeekMessage(&msg, (HWND)mHandle, 0U, 0U, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        bool handle(UINT msg, WPARAM wParam, LPARAM lParam, bool &ignore)
        {
            if (msg >= WM_MOUSEFIRST && msg <= WM_MOUSELAST) {
                InterfacesVector windowPos = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                InterfacesVector screenPos = windowPos + renderPos();
                switch (msg) {
                case WM_LBUTTONDOWN:
                    injectPointerPress(Input::PointerEventArgs { windowPos, screenPos, Input::MouseButton::LEFT_BUTTON });
                    break;
                case WM_LBUTTONUP:
                    injectPointerRelease(Input::PointerEventArgs { windowPos, screenPos, Input::MouseButton::LEFT_BUTTON });
                    break;
                case WM_RBUTTONDOWN:
                    injectPointerPress(Input::PointerEventArgs { windowPos, screenPos, Input::MouseButton::RIGHT_BUTTON });
                    break;
                case WM_RBUTTONUP:
                    injectPointerRelease(Input::PointerEventArgs { windowPos, screenPos, Input::MouseButton::RIGHT_BUTTON });
                    break;
                case WM_MBUTTONDOWN:
                    injectPointerPress(Input::PointerEventArgs { windowPos, screenPos, Input::MouseButton::MIDDLE_BUTTON });
                    break;
                case WM_MBUTTONUP:
                    injectPointerRelease(Input::PointerEventArgs { windowPos, screenPos, Input::MouseButton::MIDDLE_BUTTON });
                    break;
                case WM_MOUSEMOVE:
                    injectPointerMove(Input::PointerEventArgs { windowPos, screenPos, windowPos - mLastKnownMousePos });
                    mLastKnownMousePos = windowPos;
                    break;
                case WM_MOUSEWHEEL:
                    injectAxisEvent(Input::AxisEventArgs { Input::AxisEventArgs::WHEEL, GET_WHEEL_DELTA_WPARAM(wParam) / float(WHEEL_DELTA) });
                    break;
                }
            } else if (msg >= WM_KEYFIRST && msg <= WM_KEYLAST) {
                Input::Key::Key keycode = (Input::Key::Key)wParam;
                UINT scancode = (lParam >> 16) & 0xFF;
                switch (msg) {
                case WM_KEYDOWN:
                case WM_SYSKEYDOWN:
                    mKeyDown[keycode] = std::numeric_limits<BYTE>::max();
                    WORD buffer;
                    ToAscii(keycode, scancode, mKeyDown, &buffer, 0);
                    injectKeyPress(Input::KeyEventArgs { keycode, static_cast<char>(buffer) });
                    break;
                case WM_KEYUP:
                case WM_SYSKEYUP:
                    mKeyDown[keycode] = 0;
                    injectKeyRelease(Input::KeyEventArgs { keycode, 0 });
                    break;
                default:
                    LOG("Unknown KeyEvent " << msg);
                }
            } else {
                switch (msg) {
                case WM_SIZE:
                    onResize({ LOWORD(lParam), HIWORD(lParam) });
                    break;
                case WM_CLOSE:
                    onClose();
                    break;
                case WM_DESTROY:
                    return false;
                    break;
                case WM_PAINT: {
                    PAINTSTRUCT ps;
                    BeginPaint((HWND)mHandle, &ps);
                    onRepaint();
                    EndPaint((HWND)mHandle, &ps);
                    break;
                }
                case WM_INPUT: {
                    Input::RawInputDevice &device = Input::handleRawInput((HRAWINPUT)lParam);
                    Input::AxisEventArgs arg;
                    while (device.fetchEvent(arg))
                        injectAxisEvent(arg);
                    break;
                }
                case WM_SYSCOMMAND:
                    if (wParam == SC_KEYMENU)
                        ignore = true;
                    break;
                //default:
                    //LOG_WARNING("Unhandled Event type: " << msg);
                }
            }

            return true;
        }

        virtual InterfacesVector size() override
        {
            RECT rect;
            auto result = GetWindowRect((HWND)mHandle, &rect);
            assert(result);
            return { rect.right - rect.left, rect.bottom - rect.top };
        }

        virtual InterfacesVector renderSize() override
        {
            RECT rect;
            auto result = GetClientRect((HWND)mHandle, &rect);
            assert(result);
            return { rect.right - rect.left, rect.bottom - rect.top };
        }

        virtual void destroy() override
        {
            DestroyWindow((HWND)mHandle);
        }

        virtual InterfacesVector pos() override
        {
            RECT rect;
            auto result = GetWindowRect((HWND)mHandle, &rect);
            assert(result);
            return { rect.left, rect.top };
        }

        virtual InterfacesVector renderPos() override
        {
            POINT p { 0, 0 };
            auto result = ClientToScreen((HWND)mHandle, &p);
            assert(result);
            return { p.x, p.y };
        }

        virtual void setSize(const InterfacesVector &size) override
        {
            MoveWindow((HWND)mHandle, pos().x, pos().y, size.x, size.y, true);
        }

        virtual void setRenderSize(const InterfacesVector &size) override
        {
            RECT r;
            auto result = GetClientRect((HWND)mHandle, &r);
            assert(result);
            RECT r2;
            result = GetWindowRect((HWND)mHandle, &r2);
            assert(result);
            MoveWindow((HWND)mHandle, pos().x, pos().y, size.x + ((r2.right - r2.left) - (r.right - r.left)), size.y + ((r2.bottom - r2.top) - (r.bottom - r.top)), true);
        }
        virtual void setPos(const InterfacesVector &pos) override
        {
            MoveWindow((HWND)mHandle, pos.x, pos.y, size().x, size().y, true);
        }
        virtual void setRenderPos(const InterfacesVector &pos) override
        {
            RECT r;
            auto result = GetWindowRect((HWND)mHandle, &r);
            assert(result);
            POINT p { 0, 0 };
            result = ClientToScreen((HWND)mHandle, &p);
            assert(result);
            result = MoveWindow((HWND)mHandle, pos.x - (p.x - r.left), pos.y - (p.y - r.top), size().x, size().y, true);
            assert(result);
        }

        virtual void show() override
        {
            ShowWindow((HWND)mHandle, SW_SHOW);
        }

        virtual bool isMinimized() override
        {
            WINDOWPLACEMENT placement;
            auto result = GetWindowPlacement((HWND)mHandle, &placement);
            assert(result);
            return placement.showCmd == SW_MINIMIZE;
        }

        virtual bool isMaximized() override
        {
            WINDOWPLACEMENT placement;
            auto result = GetWindowPlacement((HWND)mHandle, &placement);
            assert(result);
            return placement.showCmd == SW_MAXIMIZE;
        }

        virtual bool isFullscreen() override
        {
            return false;
        }

        virtual void focus() override
        {
            SetFocus((HWND)mHandle);
        }
        virtual bool hasFocus() override
        {
            return GetFocus() == (HWND)mHandle;
        }

        virtual void setTitle(const char *title) override
        {
            SetWindowTextA((HWND)mHandle, title);
        }

        virtual std::string title() const override
        {
            std::string result;
            result.resize(256);
            result.resize(GetWindowTextA((HWND)mHandle, result.data(), 256));
            return result;
        }

        //Input
        virtual bool isKeyDown(Input::Key::Key key) override
        {
            return mKeyDown[key];
        }

        virtual void captureInput() override
        {
            SetCapture((HWND)mHandle);
        }

        virtual void releaseInput() override
        {
            ReleaseCapture();
        }

        virtual WindowData data() override
        {
            WINDOWPLACEMENT wndpl;
            wndpl.length = sizeof(WINDOWPLACEMENT);
            GetWindowPlacement((HWND)mHandle, &wndpl);

            return {
                { wndpl.rcNormalPosition.left, wndpl.rcNormalPosition.top },
                { wndpl.rcNormalPosition.right - wndpl.rcNormalPosition.left,
                    wndpl.rcNormalPosition.bottom - wndpl.rcNormalPosition.top },
                wndpl.showCmd == SW_MAXIMIZE,
                isFullscreen()
            };
        }

    private:
        BYTE mKeyDown[512];
        InterfacesVector mLastKnownMousePos;
    };

    static std::unordered_map<HWND, WindowsWindow> sWindows;

    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    static const char *CreateWindowClass()
    {
        HINSTANCE hInstance = GetModuleHandle(nullptr);

        // Register class
        WNDCLASSEX wcex;
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wcex.lpfnWndProc = WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hInstance;
        wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_APPLICATION);
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = nullptr;
        wcex.lpszClassName = "InterfacesWindowClass";
        wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_APPLICATION);
        if (!RegisterClassEx(&wcex))
            return nullptr;
        return wcex.lpszClassName;
    }

    OSWindow *sCreateWindow(const WindowSettings &settings)
    {
        HWND handle = (HWND)settings.mHandle;
        if (!handle) {
            static const char *windowClass = CreateWindowClass();

            DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_SIZEBOX | WS_MAXIMIZEBOX;
            if (settings.mHeadless) {
                style = WS_POPUP;
            }

            // Create window
            HINSTANCE hInstance = GetModuleHandle(nullptr);
            RECT rc = { 0, 0, (LONG)settings.mData.mSize.x, (LONG)settings.mData.mSize.y };
            AdjustWindowRect(&rc, style, FALSE);
            int x = CW_USEDEFAULT, y = CW_USEDEFAULT;
            if (settings.mData.mPosition.x >= 0 && settings.mData.mPosition.y >= 0) {
                x = settings.mData.mPosition.x;
                y = settings.mData.mPosition.y;
            }

            handle = CreateWindow(windowClass, settings.mTitle,
                style,
                x, y, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
                nullptr);
            if (!handle)
                return nullptr;
        }

        if (!settings.mHidden) {
            UINT showCmd = settings.mData.mMaximized ? SW_SHOWMAXIMIZED : SW_SHOW;
            if (settings.mData.mPosition.x >= 0 && settings.mData.mPosition.y >= 0) {
                WINDOWPLACEMENT wndpl {
                    sizeof(WINDOWPLACEMENT),
                    0,
                    showCmd,
                    { -1, -1 },
                    { -1, -1 },
                    { settings.mData.mPosition.x, settings.mData.mPosition.y,
                        settings.mData.mPosition.x + settings.mData.mSize.x,
                        settings.mData.mPosition.y + settings.mData.mSize.y }
                };
                SetWindowPlacement(handle, &wndpl);
            } else {
                ShowWindow(handle, showCmd);
            }
        }

        Input::setupRawInput(handle);

        auto pib = sWindows.try_emplace(handle, handle);
        assert(pib.second);

        return &pib.first->second;
    }

    OSWindow *sFromNative(uintptr_t handle)
    {
        return handle ? &sWindows.at((HWND)handle) : nullptr;
    }

    static std::vector<MonitorInfo> sBuffer;

    static BOOL __stdcall MonitorEnumerator(HMONITOR, HDC, LPRECT rect, LPARAM)
    {
        sBuffer.push_back({ rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top });
        return BOOL(true);
    }

    static void updateMonitors()
    {
        sBuffer.clear();
        auto result = EnumDisplayMonitors(
            NULL, NULL, &MonitorEnumerator,
            NULL);
        assert(result);
    }

    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        bool ignore = false;
        auto it = sWindows.find(hwnd);
        if (it != sWindows.end()) {
            if (msg == WM_DISPLAYCHANGE) {
                if (it == sWindows.begin())
                    updateMonitors();
            } else if (!it->second.handle(msg, wParam, lParam, ignore))
                sWindows.erase(it);
        }
        if (ignore)
            return 0;
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    std::vector<MonitorInfo> listMonitors()
    {
        if (sBuffer.empty())
            updateMonitors();
        return sBuffer;
    }

}
}

#endif
