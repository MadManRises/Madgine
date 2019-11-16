#pragma once

#include "widget.h"

namespace Engine
{
namespace Widgets
	{
    class MADGINE_WIDGETS_EXPORT TabWidget : public Widget<TabWidget>
		{
		public:			
			using Widget::Widget;
			virtual ~TabWidget() = default;

			virtual void addTab(Widget* w);

			virtual WidgetClass getClass() const override;
		};
	}
}
