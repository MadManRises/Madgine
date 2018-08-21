#pragma once

#include "widget.h"

namespace Engine
{
	namespace GUI
	{
		class Textbox : public virtual Widget
		{
		public:		
			using Widget::Widget;
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
