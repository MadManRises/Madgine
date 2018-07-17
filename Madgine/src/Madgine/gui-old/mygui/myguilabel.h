#pragma once
#include "../windows/label.h"

namespace MyGUI
{
	class TextBox;
}

namespace Engine
{
	namespace GUI
	{
		namespace MyGui
		{
			class MyGUIWindow;

			class MyGUILabel :
				public Label
			{
			public:
				MyGUILabel(MyGUIWindow* w);

				// Inherited via Label
				void setText(const std::string& text) override;

			private:
				MyGUI::TextBox* mLabel;
			};
		}
	}
}
