#pragma once

#include "window.h"

namespace Engine
{
	namespace GUI
	{
		class Textbox : public Window
		{
		public:
			using Window::Window;
			virtual ~Textbox() = default;

			virtual std::string getText() = 0;
			virtual void setText(const std::string& text) = 0;
			virtual void setEditable(bool b) = 0;

			void clear()
			{
				setText("");
			}
		};
	}
}
