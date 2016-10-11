#pragma once

#include "window.h"

namespace Engine {
	namespace GUI {

		class Checkbox : public Window
		{
		public:
			using Window::Window;
			virtual ~Checkbox() = default;

			virtual bool isChecked() = 0;
			virtual void setChecked(bool b) = 0;
		};

	}
}