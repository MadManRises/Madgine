#pragma once

#include "Madgine/gui/windows/textbox.h"
#include "myguiwindow.h"

namespace Engine
{
	namespace GUI
	{

		class MyGUITextbox : public Textbox, public MyGUIWindow
		{
		public:
			MyGUITextbox(const std::string &name, MyGUIWindow *parent);
			MyGUITextbox(const std::string &name, MyGUISystem &system);

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
