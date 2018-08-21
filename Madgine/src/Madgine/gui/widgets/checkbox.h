#pragma once

#include "widget.h"

namespace Engine
{
	namespace GUI
	{
		class Checkbox : public virtual Widget
		{
		public:
			using Widget::Widget;
			virtual ~Checkbox() = default;

			virtual bool isChecked() = 0;
			virtual void setChecked(bool b) = 0;
		};
	}
}
