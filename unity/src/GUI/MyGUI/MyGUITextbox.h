#pragma once

#include "GUI/Windows/Textbox.h"
#include "MyGUIWindow.h"

namespace Engine {
	namespace GUI {
		namespace MyGui {
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


