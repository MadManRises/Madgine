#pragma once

#include "GUI\Windows\Combobox.h"

namespace MyGUI {
	class ComboBox;
}

namespace Engine {
	namespace GUI {
		namespace MyGui {

			class MyGUIWindow;

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

