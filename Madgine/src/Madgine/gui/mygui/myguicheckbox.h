#pragma once

#include "../windows/checkbox.h"

namespace MyGUI
{
	class Widget;
	class Button;
}

namespace Engine
{
	namespace GUI
	{
		namespace MyGui
		{
			class MyGUIWindow;

			class MyGUICheckbox : public Checkbox
			{
			public:
				MyGUICheckbox(MyGUIWindow* w);

				// Inherited via Checkbox
				bool isChecked() override;
				void setChecked(bool b) override;

			private:
				void toggle(MyGUI::Widget* t);

				MyGUI::Button* mCheckbox;
			};
		}
	}
}
