#pragma once

#include "window.h"

namespace Engine
{
	namespace GUI
	{
		class Bar : public Window
		{
		public:
			using Window::Window;

			virtual void setRatio(float f) = 0;
		};
	}
}
