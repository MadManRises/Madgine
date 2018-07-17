#pragma once

#include "Madgine/gui/windows/button.h"
#include "myguiwindow.h"

namespace Engine
{
	namespace GUI
	{

		class MyGUIButton : public Button, public MyGUIWindow
		{
		public:
			MyGUIButton(const std::string &name, MyGUIWindow *parent);
			MyGUIButton(const std::string &name, MyGUISystem &system);

			void setText(const std::string& text) override;
			Class getClass() override;

		protected:
			MyGUI::Widget* widget() const override;

			void clicked(MyGUI::Widget *sender);

		private:
			MyGUI::Button *mButton;
		};

	}
}