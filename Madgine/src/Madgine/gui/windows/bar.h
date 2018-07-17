#pragma once

#include "window.h"

namespace Engine
{
	namespace GUI
	{
		class Bar : public virtual Window
		{
		public:
			using Window::Window;
			virtual ~Bar() = default;

			virtual void setRatio(float f) = 0;
		};
	}
}
