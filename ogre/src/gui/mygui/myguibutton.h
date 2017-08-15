#pragma once
#include "gui/windows/button.h"

namespace MyGUI {
	class Button;
}

namespace Engine {
	namespace GUI {
		namespace MyGui {

			class MyGUIWindow;

			class MyGUIButton :
				public Button
			{
			public:
				MyGUIButton(MyGUIWindow *w);

				virtual void setText(const std::string &text) override;

			private:
				MyGUI::Button *mButton;
			};
		}
	}
}


