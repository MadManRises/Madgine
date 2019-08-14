#pragma once

#include "widget.h"

namespace Engine
{
	namespace GUI
	{
		class Combobox : public Widget
		{
		public:
			using Widget::Widget;
			virtual ~Combobox() = default;

			virtual void addItem(const std::string& text);
			virtual void clear();

			virtual void setText(const std::string& s);
			virtual std::string getText();
		};
	}
}
