#pragma once
#include "GUI\Windows\Bar.h"
#include "MyGUIWindow.h"

namespace Engine {
	namespace GUI {
		namespace MyGui {
			class MyGUIBar :
				public Bar
			{
			public:
				MyGUIBar(MyGUIWindow *w);

				// Inherited via Bar
				virtual void setRatio(float f) override;

			private:
				MyGUI::ProgressBar *mBar;
			};
		}
	}
}


