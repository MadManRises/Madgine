#pragma once

#include "window.h"

namespace Engine
{
	namespace GUI
	{
		class MADGINE_CLIENT_EXPORT Button : public Window
		{
		public:
			using Window::Window;
			virtual ~Button() = default;
			virtual void setText(const std::string& text) = 0;

			void registerOnClickEvent(void* id, std::function<void()> f)
			{
				registerEvent(id, EventType::ButtonClick, f);
			};
		};
	}
}
