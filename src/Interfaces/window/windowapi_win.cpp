#include "../interfaceslib.h"

#if WINDOWS

#    include "windowapi.h"
#    include "windoweventlistener.h"
#    include "windowsettings.h"

#    include "../input/inputevents.h"

#    define NOMINMAX
#    include <Windows.h>
#    include <hidsdi.h>
#    include <windowsx.h>

namespace Engine {
namespace Window {

#    define USB_PACKET_LENGTH 64

    DLL_EXPORT const PlatformCapabilities platformCapabilities {
        true,
        1.0f
    };

    void setupRawInput(HWND handle);
    void handleRawInput(PRAWINPUT input);

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

        bool handle(UINT msg, WPARAM wParam, LPARAM lParam)
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
                    injectPointerMove(Input::PointerEventArgs { windowPos - renderPos(), windowPos, { 0, 0 }, GET_WHEEL_DELTA_WPARAM(wParam) / float(WHEEL_DELTA) });
                    break;
                }
            } else if (msg >= WM_KEYFIRST && msg <= WM_KEYLAST) {
                Input::Key::Key keycode = (Input::Key::Key)wParam;
                UINT scancode = (lParam >> 16) & 0xFF;
                switch (msg) {
                case WM_KEYDOWN:
                    mKeyDown[keycode] = std::numeric_limits<BYTE>::max();
                    WORD buffer;
                    ToAscii(keycode, scancode, mKeyDown, &buffer, 0);
                    injectKeyPress(Input::KeyEventArgs { keycode, static_cast<char>(buffer) });
                    break;
                case WM_KEYUP:
                    mKeyDown[keycode] = 0;
                    injectKeyRelease(Input::KeyEventArgs { keycode, 0 });
                    break;
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
                    std::byte data[sizeof(RAWINPUTHEADER) + sizeof(RAWHID) + USB_PACKET_LENGTH];
                    UINT buffer_size = sizeof(data);

                    if ((int)GetRawInputData((HRAWINPUT)lParam, RID_INPUT, data, &buffer_size, sizeof(RAWINPUTHEADER)) > 0) {
                        PRAWINPUT raw_input = (PRAWINPUT)data;
                        handleRawInput(raw_input);
                    }
                } break;
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

        setupRawInput(handle);

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
        auto it = sWindows.find(hwnd);
        if (it != sWindows.end()) {
            if (msg == WM_DISPLAYCHANGE) {
                if (it == sWindows.begin())
                    updateMonitors();
            } else if (!it->second.handle(msg, wParam, lParam))
                sWindows.erase(it);
        }
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    std::vector<MonitorInfo> listMonitors()
    {
        if (sBuffer.empty())
            updateMonitors();
        return sBuffer;
    }

    ////RAWINPUT


    static std::mutex sRawInputDevicesMutex;
    static std::atomic_flag sRawInputInitialized = ATOMIC_FLAG_INIT;

    decltype(&HidD_GetManufacturerString) sGetManufacturerString;
    decltype(&HidD_GetProductString) sGetProductString;
    decltype(&HidD_GetPreparsedData) sGetPreparsedData;
    decltype(&HidP_GetCaps) sGetCaps;
    decltype(&HidP_GetValueCaps) sGetValueCaps;
    decltype(&HidP_GetButtonCaps) sGetButtonCaps;
    decltype(&HidP_GetUsageValue) sGetUsageValue;
    decltype(&HidP_GetData) sGetData;
    decltype(&HidP_MaxDataListLength) sMaxDataListLength;

    void loadHID(void)
    {
        HMODULE handle = LoadLibrary("hid.dll");
        assert(handle);

        sGetManufacturerString = (decltype(&HidD_GetManufacturerString))GetProcAddress(handle, "HidD_GetManufacturerString");
        sGetProductString = (decltype(&HidD_GetProductString))GetProcAddress(handle, "HidD_GetProductString");
        sGetPreparsedData = (decltype(&HidD_GetPreparsedData))GetProcAddress(handle, "HidD_GetPreparsedData");
        sGetCaps = (decltype(&HidP_GetCaps))GetProcAddress(handle, "HidP_GetCaps");
        sGetData = (decltype(&HidP_GetData))GetProcAddress(handle, "HidP_GetData");
        sGetValueCaps = (decltype(&HidP_GetValueCaps))GetProcAddress(handle, "HidP_GetValueCaps");
        sGetButtonCaps = (decltype(&HidP_GetButtonCaps))GetProcAddress(handle, "HidP_GetButtonCaps");
        sGetUsageValue = (decltype(&HidP_GetUsageValue))GetProcAddress(handle, "HidP_GetUsageValue");
        sMaxDataListLength = (decltype(&HidP_MaxDataListLength))GetProcAddress(handle, "HidP_MaxDataListLength");

    }

    struct RawInputDevice {
        RawInputDevice(HANDLE handle, std::string manufacturer, std::string product, PHIDP_PREPARSED_DATA preparsedData)
            : mHandle(handle)
            , mManufacturer(std::move(manufacturer))
            , mProduct(std::move(product))
            , mPreparsedData(preparsedData)
        {
            HIDP_CAPS caps;
            auto result = sGetCaps(mPreparsedData, &caps);
            assert(result == HIDP_STATUS_SUCCESS);

            std::unique_ptr<HIDP_BUTTON_CAPS[]> buttonCaps = std::make_unique<HIDP_BUTTON_CAPS[]>(caps.NumberInputButtonCaps);

            result = sGetButtonCaps(HidP_Input, buttonCaps.get(), &caps.NumberInputButtonCaps, mPreparsedData);
            assert(result == HIDP_STATUS_SUCCESS);

            std::unique_ptr<HIDP_VALUE_CAPS[]> valueCaps = std::make_unique<HIDP_VALUE_CAPS[]>(caps.NumberInputValueCaps);

            result = sGetValueCaps(HidP_Input, valueCaps.get(), &caps.NumberInputValueCaps, mPreparsedData);
            assert(result == HIDP_STATUS_SUCCESS);


        }

        HANDLE mHandle;
        std::string mManufacturer;
        std::string mProduct;

        PHIDP_PREPARSED_DATA mPreparsedData;
    };

    std::map<HANDLE, RawInputDevice> sRawInputDevices;

    void addRawInputDevice(HANDLE device)
    {

        if (sRawInputDevices.count(device) > 0)
            return;

        RID_DEVICE_INFO rdi;
        UINT rdi_size = sizeof(rdi);

        auto result = GetRawInputDeviceInfoA(device, RIDI_DEVICEINFO, &rdi, &rdi_size);
        assert(result > 0);
        assert(rdi.dwType == RIM_TYPEHID);

        if (rdi.hid.usUsagePage != HID_USAGE_PAGE_GENERIC || (rdi.hid.usUsage != HID_USAGE_GENERIC_JOYSTICK && rdi.hid.usUsage != HID_USAGE_GENERIC_GAMEPAD))
            return;

        char device_name[MAX_PATH];
        UINT name_size = sizeof(device_name);
        result = GetRawInputDeviceInfoA(device, RIDI_DEVICENAME, device_name, &name_size);
        assert(result > 0);

        HANDLE hFile = CreateFileA(device_name, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
        assert(hFile != INVALID_HANDLE_VALUE);

        std::string manufacturer, product;
        {
            WCHAR string[128];

            if (sGetManufacturerString(hFile, string, sizeof(string))) {
                std::wstring ws = string;
                manufacturer = { ws.begin(), ws.end() };
            }
            if (sGetProductString(hFile, string, sizeof(string))) {
                std::wstring ws = string;
                product = { ws.begin(), ws.end() };
            }

            //device->name = SDL_CreateJoystickName(device->vendor_id, device->product_id, manufacturer_string, product_string);
        }

        PHIDP_PREPARSED_DATA preparsedData;
        result = sGetPreparsedData(hFile, &preparsedData);
        assert(result);

        //CHECK(SDL_HidD_GetPreparsedData(hFile, &device->preparsed_data));
        CloseHandle(hFile);
        hFile = INVALID_HANDLE_VALUE;

        sRawInputDevices.try_emplace(device, device, std::move(manufacturer), std::move(product), preparsedData);
    }

    void listRawInputDevices()
    {
        std::scoped_lock lock { sRawInputDevicesMutex };

        UINT device_count = 0;

        if ((GetRawInputDeviceList(NULL, &device_count, sizeof(RAWINPUTDEVICELIST)) != -1) && device_count > 0) {
            std::unique_ptr<RAWINPUTDEVICELIST[]> devices = std::make_unique<RAWINPUTDEVICELIST[]>(device_count);

            if (GetRawInputDeviceList(devices.get(), &device_count, sizeof(RAWINPUTDEVICELIST)) != -1) {
                for (size_t i = 0; i < device_count; ++i) {
                    if (devices[i].dwType == RIM_TYPEHID)
                        addRawInputDevice(devices[i].hDevice);
                }
            }
        }
    }

    void setupRawInput(HWND handle)
    {
        if (!sRawInputInitialized.test_and_set()) {
            loadHID();
            listRawInputDevices();

            RAWINPUTDEVICE rid[2];

            rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
            rid[0].usUsage = HID_USAGE_GENERIC_JOYSTICK;
            rid[0].dwFlags = RIDEV_DEVNOTIFY; /* Receive messages when in background, including device add/remove */
            rid[0].hwndTarget = NULL;

            rid[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
            rid[1].usUsage = HID_USAGE_GENERIC_GAMEPAD;
            rid[1].dwFlags = RIDEV_DEVNOTIFY; /* Receive messages when in background, including device add/remove */
            rid[1].hwndTarget = NULL;

            auto result = RegisterRawInputDevices(rid, 2, sizeof(RAWINPUTDEVICE));
            assert(result);
        }
    }

    void handleRawInputStatePacket(RawInputDevice &device, BYTE *data, int bytesize, int count)
    {
        LOG(device.mProduct << "(" << bytesize << ")");

        ULONG size = sMaxDataListLength(HidP_Input, device.mPreparsedData);
        std::unique_ptr<HIDP_DATA[]> list = std::make_unique<HIDP_DATA[]>(size);

        auto result = sGetData(HidP_Input, list.get(), &size, device.mPreparsedData, (PCHAR)data, bytesize);
        assert(result = HIDP_STATUS_SUCCESS);
        
        {
            Util::LogDummy out { Util::LOG_TYPE };
            for (int i = 0; i < size; ++i) {
                out << list[i].RawValue << "(" << list[i].DataIndex << "), ";
            }
        }

        ULONG value;
        result = sGetUsageValue(HidP_Input, HID_USAGE_PAGE_GENERIC, 0, HID_USAGE_GENERIC_Z, &value, device.mPreparsedData, (PCHAR)data, bytesize);
        assert(result = HIDP_STATUS_SUCCESS);
        LOG("GENERIC_Z: " << value);

        result = sGetUsageValue(HidP_Input, HID_USAGE_PAGE_GENERIC, 0, HID_USAGE_GENERIC_RZ, &value, device.mPreparsedData, (PCHAR)data, bytesize);
        assert(result = HIDP_STATUS_SUCCESS);
        LOG("GENERIC_RZ: " << value);

        result = sGetUsageValue(HidP_Input, HID_USAGE_PAGE_GENERIC, 0, HID_USAGE_GENERIC_X, &value, device.mPreparsedData, (PCHAR)data, bytesize);
        assert(result = HIDP_STATUS_SUCCESS);
        LOG("GENERIC_X: " << value);
    }

    void handleRawInput(PRAWINPUT input)
    {
        auto it = sRawInputDevices.find(input->header.hDevice);        
        if (it != sRawInputDevices.end()) {            
            handleRawInputStatePacket(it->second, input->data.hid.bRawData, input->data.hid.dwSizeHid, input->data.hid.dwCount);
        }
    }

}
}

#endif
