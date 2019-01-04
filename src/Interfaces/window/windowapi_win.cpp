#include "../interfaceslib.h"

#ifdef _WIN32

#include "windowapi.h"
#include "windoweventlistener.h"

#define NOMINMAX
#include <Windows.h>

namespace Engine {
	namespace Window {

		struct WindowsWindow : Window {
			WindowsWindow(HWND hwnd) :
				Window((uintptr_t)hwnd)
			{				
			}

			bool handle(UINT msg, WPARAM wParam, LPARAM lParam) {				
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
			}

			virtual size_t width() override {
				RECT rect;
				auto result = GetWindowRect((HWND)mHandle, &rect);
				assert(result);
				return rect.right - rect.left;
			}


			virtual size_t height() override {
				RECT rect;
				auto result = GetWindowRect((HWND)mHandle, &rect);
				assert(result);
				return rect.bottom - rect.top;
			}

			virtual size_t renderWidth() override {
				RECT rect;
				auto result = GetClientRect((HWND)mHandle, &rect);
				assert(result);
				return rect.right - rect.left;
			}

			virtual size_t renderHeight() override {
				RECT rect;
				auto result = GetClientRect((HWND)mHandle, &rect);
				assert(result);
				return rect.bottom - rect.top;
			}

			virtual void beginFrame() override {
				BeginPaint((HWND)mHandle, &mPs);
			}

			virtual void endFrame() override {
				EndPaint((HWND)mHandle, &mPs);
				SwapBuffers(GetDC((HWND)mHandle));				
			}

			virtual void destroy() override {
				DestroyWindow((HWND)mHandle);
			}

			PAINTSTRUCT mPs;
				
		};

		static std::unordered_map<HWND, WindowsWindow> sWindows;


		LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		static const char *CreateWindowClass() {
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

		Window *sCreateWindow(const WindowSettings &settings) {
			HWND handle = (HWND)settings.mHandle;
			if (!handle) {
				static const char *windowClass = CreateWindowClass();

				// Create window
				HINSTANCE hInstance = GetModuleHandle(nullptr);
				RECT rc = { 0, 0, (LONG)settings.mWidth, (LONG)settings.mHeight };
				AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
				handle = CreateWindow(windowClass, settings.mTitle,
					WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_SIZEBOX | WS_MAXIMIZEBOX,
					CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
					nullptr);
				if (!handle)
					return nullptr;
			}

			ShowWindow(handle, SW_SHOW);

			auto pib = sWindows.try_emplace(handle, handle);
			assert(pib.second);

			return &pib.first->second;
		}	

		void sUpdate() {
			MSG  msg;
			while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
		{
			auto it = sWindows.find(hwnd);
			if (it != sWindows.end()) {
				if (!it->second.handle(msg, wParam, lParam))
					sWindows.erase(it);
			}
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}

	}
}


#endif