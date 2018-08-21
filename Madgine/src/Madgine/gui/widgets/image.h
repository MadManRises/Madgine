#pragma once

#include "widget.h"

namespace Engine
{
	namespace GUI
	{
		class Image : public virtual Widget
		{
		public:
			using Widget::Widget;
			virtual ~Image() = default;

			virtual void setImage(const std::string &name) = 0;
			virtual std::string getImage() const = 0;
		};
	}
}
