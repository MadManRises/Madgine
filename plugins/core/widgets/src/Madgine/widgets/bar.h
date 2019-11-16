#pragma once

#include "widget.h"

namespace Engine
{
namespace Widgets
	{
    class MADGINE_WIDGETS_EXPORT Bar : public Widget<Bar>
		{
		public:
			using Widget::Widget;
			virtual ~Bar() = default;

			virtual void setRatio(float f);

			virtual WidgetClass getClass() const override;
		};
	}
}
