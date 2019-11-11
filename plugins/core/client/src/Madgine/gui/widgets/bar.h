#pragma once

#include "widget.h"

namespace Engine
{
namespace Widgets
	{
		class Bar : public Widget<Bar>
		{
		public:
			using Widget::Widget;
			virtual ~Bar() = default;

			virtual void setRatio(float f);

			virtual WidgetClass getClass() const override;
		};
	}
}
