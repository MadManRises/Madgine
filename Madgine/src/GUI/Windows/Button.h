#pragma once

#include "Window.h"

namespace Engine {
	namespace GUI {

		class Button : public Window {
		public:
			using Window::Window;
			virtual ~Button() = default;
			inline void registerOnClickEvent(void *id, std::function<void()> f) {
				registerEvent(id, EventType::ButtonClick, f);
			};

		};

	}
}
