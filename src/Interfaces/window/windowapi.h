#pragma once

#include "windoweventlistener.h"

namespace Engine {
	namespace Window {

		struct WindowSettings {
			void *mHandle = nullptr;

			size_t mWidth = 800;
			size_t mHeight = 600;

			const char *mTitle = "Interfaces - Window";
		};

		struct INTERFACES_EXPORT Window {

			Window(void *handle) : mHandle(handle) {}

			void addListener(WindowEventListener *listener) {
				mListeners.push_back(listener);
			}

			void removeListener(WindowEventListener *listener) {
				mListeners.erase(std::remove(mListeners.begin(), mListeners.end(), listener), mListeners.end());
			}

			virtual void beginFrame() = 0;
			virtual void endFrame() = 0;

			virtual size_t width() = 0;
			virtual size_t height() = 0;

			virtual size_t renderWidth() = 0;
			virtual size_t renderHeight() = 0;

			virtual void destroy() = 0;

			const void *const mHandle;

		protected:
			void onResize(size_t width, size_t height) {
				for (WindowEventListener *listener : mListeners)
					listener->onResize(width, height);
			}

			void onClose() {
				for (WindowEventListener *listener : mListeners)
					listener->onClose();
			}

			void onRepaint() {
				for (WindowEventListener *listener : mListeners)
					listener->onRepaint();
			}

		private:
			std::vector<WindowEventListener*> mListeners;
		};

		INTERFACES_EXPORT Window * sCreateWindow(const WindowSettings &settings);
		INTERFACES_EXPORT void sUpdate();

	}
}