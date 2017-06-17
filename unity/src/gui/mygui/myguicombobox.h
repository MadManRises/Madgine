#pragma once

#include "GUI\Windows\Combobox.h"
#include "MyGUIWindow.h"


namespace Engine {
	namespace GUI {
		namespace MyGui {

			class MyGUICombobox :
				public Combobox
			{
			public:
				MyGUICombobox(MyGUIWindow *w);

				// Inherited via Combobox
				virtual void addItem(const std::string & text) override;
				virtual void clear() override;
				virtual void setText(const std::string & s) override;
				virtual std::string getText() override;

			private:
				MyGUI::ComboBox *mCombobox;
			};

		}
	}
}

