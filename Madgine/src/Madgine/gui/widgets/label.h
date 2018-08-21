#pragma once

#include "widget.h"

namespace Engine
{
	namespace GUI
	{
		class Label : public virtual Widget
		{
		public:
			using Widget::Widget;
			virtual ~Label() = default;

			virtual void setText(const std::string& text) = 0;
		};
	}
}
