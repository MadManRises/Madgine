#pragma once

#include "window.h"

namespace Engine
{
	namespace GUI
	{
		class Image : public virtual Window
		{
		public:
			using Window::Window;
			virtual ~Image() = default;

			virtual void setImage(const std::string &name) = 0;
		};
	}
}
