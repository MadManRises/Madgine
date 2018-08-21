#pragma once

#include "widget.h"

namespace Engine
{
	namespace GUI
	{
		class Combobox : public virtual Widget
		{
		public:
			using Widget::Widget;
			virtual ~Combobox() = default;

			virtual void addItem(const std::string& text) = 0;
			virtual void clear() = 0;

			virtual void setText(const std::string& s) = 0;
			virtual std::string getText() = 0;
		};
	}
}
