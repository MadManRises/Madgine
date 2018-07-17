#pragma once

#include "window.h"

namespace Engine
{
	namespace GUI
	{
		class TabWindow : public virtual Window
		{
		public:			
			using Window::Window;
			virtual ~TabWindow() = default;

			virtual void addTab(Window* w) = 0;
		};
	}
}
