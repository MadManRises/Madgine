#pragma once

#include "Madgine/gui/windows/label.h"
#include "myguiwindow.h"

namespace Engine
{
	namespace GUI
	{

		class MyGUILabel : public Label, public MyGUIWindow
		{
		public:
			MyGUILabel(const std::string &name, MyGUIWindow *parent);
			MyGUILabel(const std::string &name, MyGUISystem &system);

			void setText(const std::string& text) override;

			Class getClass() override;

		protected:
			MyGUI::Widget* widget() const override;

		private:
			MyGUI::TextBox *mTextBox;
		};

	}
}