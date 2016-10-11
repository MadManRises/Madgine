#pragma once

#include "window.h"

namespace Engine {
	namespace GUI {
		class Layout : public Window{
		public:
			using Window::Window;
			virtual void layout() = 0;
		};
	}
}