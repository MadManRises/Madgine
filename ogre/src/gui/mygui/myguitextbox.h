#pragma once

#include "gui/windows/textbox.h"

namespace MyGUI {
	class EditBox;
}

namespace Engine {
	namespace GUI {
		namespace MyGui {

			class MyGUIWindow;

			class MyGUITextbox : public Textbox
			{
			public:
				MyGUITextbox(MyGUIWindow *w);

				// Inherited via Textbox
				virtual std::string getText() override;
				virtual void setText(const std::string & text) override;
				virtual void setEditable(bool b) override;

			private:
				MyGUI::EditBox *mTextbox;
			};
		}
	}
}


