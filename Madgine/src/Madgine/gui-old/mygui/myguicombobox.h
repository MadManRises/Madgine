#pragma once

#include "../windows/combobox.h"

namespace MyGUI
{
	class ComboBox;
}

namespace Engine
{
	namespace GUI
	{
		namespace MyGui
		{
			class MyGUIWindow;

			class MyGUICombobox :
				public Combobox
			{
			public:
				MyGUICombobox(MyGUIWindow* w);

				// Inherited via Combobox
				void addItem(const std::string& text) override;
				void clear() override;
				void setText(const std::string& s) override;
				std::string getText() override;

			private:
				MyGUI::ComboBox* mCombobox;
			};
		}
	}
}
