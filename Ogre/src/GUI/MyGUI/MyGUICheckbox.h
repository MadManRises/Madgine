#pragma once

#include "GUI\Windows\Checkbox.h"
#include "MyGUIWindow.h"

namespace MyGUI {
	class Widget;
}

namespace Engine {
	namespace GUI {
		namespace MyGui {
			class MyGUICheckbox : public Checkbox
			{
			public:
				MyGUICheckbox(MyGUIWindow *w);

				// Inherited via Checkbox
				virtual bool isChecked() override;
				virtual void setChecked(bool b) override;

			private:
				void toggle(MyGUI::Widget *t);

				MyGUI::Button *mCheckbox;
			};
		}
	}
}


