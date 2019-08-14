#pragma once

namespace Engine {

	namespace Window {

		struct WindowEventListener {
			virtual void onResize(size_t width, size_t height) {};
			virtual void onClose() {};
			virtual void onRepaint() {};
			
		};

	}

}