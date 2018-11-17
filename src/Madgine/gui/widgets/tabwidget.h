#pragma once

#include "widget.h"

namespace Engine
{
	namespace GUI
	{
		class TabWidget : public virtual Widget
		{
		public:			
			using Widget::Widget;
			virtual ~TabWidget() = default;

			virtual void addTab(Widget* w) = 0;
		};
	}
}
