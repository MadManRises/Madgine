#pragma once
#include "GUI\Windows\Label.h"
#include "MyGUIWindow.h"

namespace Engine {
	namespace GUI {
		namespace MyGui {
			class MyGUILabel :
				public Label
			{
			public:
				MyGUILabel(MyGUIWindow *w);

				// Inherited via Label
				virtual void setText(const std::string & text) override;

			private:
				MyGUI::TextBox *mLabel;
			};
		}
	}
}
