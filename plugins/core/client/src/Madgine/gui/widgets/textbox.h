#pragma once

#include "widget.h"

namespace Engine
{
	namespace GUI
	{
		class Textbox : public Widget<Textbox>
		{
		public:		
			using Widget::Widget;
			virtual ~Textbox() = default;

			virtual std::string getText();
			virtual void setText(const std::string& text);
			virtual void setEditable(bool b);

			void clear()
			{
				setText("");
			}
		};
	}
}
