#pragma once

#include "window.h"

namespace Engine
{
	namespace GUI
	{
		class Combobox : public virtual Window
		{
		public:
			using Window::Window;
			virtual ~Combobox() = default;

			virtual void addItem(const std::string& text) = 0;
			virtual void clear() = 0;

			virtual void setText(const std::string& s) = 0;
			virtual std::string getText() = 0;
		};
	}
}
