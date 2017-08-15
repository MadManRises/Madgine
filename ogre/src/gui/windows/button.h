#pragma once

#include "window.h"

namespace Engine {
	namespace GUI {

		class OGREMADGINE_EXPORT Button : public Window {
		public:
			using Window::Window;
			virtual ~Button() = default;
			virtual void setText(const std::string &text) = 0;
			inline void registerOnClickEvent(void *id, std::function<void()> f) {
				registerEvent(id, EventType::ButtonClick, f);
			};

		};

	}
}
