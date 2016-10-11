#pragma once
#include "GUI\Windows\Layout.h"
#include "MyGUIWindow.h"

namespace Engine {
	namespace GUI {
		namespace MyGui {
			class MyGUILayout :
				public Layout
			{
			public:
				MyGUILayout(MyGUIWindow *w);

				// Inherited via Layout
				virtual void layout() override;
			};
		}
	}
}


