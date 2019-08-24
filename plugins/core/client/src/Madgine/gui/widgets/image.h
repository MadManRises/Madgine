#pragma once

#include "widget.h"

namespace Engine
{
	namespace GUI
	{
		class Image : public Widget
		{
		public:
			using Widget::Widget;
			virtual ~Image() = default;

			virtual void setImage(const std::string &name);
			virtual std::string getImage() const;
		};
	}
}
