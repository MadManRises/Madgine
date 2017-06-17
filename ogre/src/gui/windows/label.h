#pragma once

#include "window.h"

namespace Engine {
	namespace GUI {
		class Label : public Window {
		public:
			using Window::Window;
			virtual ~Label() = default;

			virtual void setText(const std::string &text) = 0;
		};
	}
}