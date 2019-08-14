#pragma once

#include "widget.h"

namespace Engine
{
	namespace GUI
	{
		class Bar : public Widget
		{
		public:
			using Widget::Widget;
			virtual ~Bar() = default;

			virtual void setRatio(float f);
		};
	}
}
