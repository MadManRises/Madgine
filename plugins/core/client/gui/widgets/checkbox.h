#pragma once

#include "widget.h"

namespace Engine
{
	namespace GUI
	{
		class Checkbox : public Widget
		{
		public:
			using Widget::Widget;
			virtual ~Checkbox() = default;

			virtual bool isChecked();
			virtual void setChecked(bool b);
		};
	}
}
