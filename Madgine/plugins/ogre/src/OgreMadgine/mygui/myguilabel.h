#pragma once

#include "Madgine/gui/widgets/label.h"
#include "myguiwidget.h"

namespace Engine
{
	namespace GUI
	{

		class MyGUILabel : public Label, public MyGUIWidget
		{
		public:
			MyGUILabel(const std::string &name, MyGUIWidget *parent);
			MyGUILabel(const std::string &name, MyGUITopLevelWindow &system);

			void setText(const std::string& text) override;

			Class getClass() override;

		protected:
			MyGUI::Widget* widget() const override;

		private:
			MyGUI::TextBox *mTextBox;
		};

	}
}