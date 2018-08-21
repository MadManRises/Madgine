#pragma once

#include "Madgine/gui/widgets/textbox.h"
#include "myguiwidget.h"

namespace Engine
{
	namespace GUI
	{

		class MyGUITextbox : public Textbox, public MyGUIWidget
		{
		public:
			MyGUITextbox(const std::string &name, MyGUIWidget *parent);
			MyGUITextbox(const std::string &name, MyGUITopLevelWindow &window);

			std::string getText() override;
			void setText(const std::string& text) override;
			void setEditable(bool b) override;

			Class getClass() override;

		protected:
			MyGUI::Widget* widget() const override;

		private:
			MyGUI::EditBox *mTextbox;
		};

	}
}
