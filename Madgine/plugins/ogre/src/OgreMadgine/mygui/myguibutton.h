#pragma once

#include "Madgine/gui/widgets/button.h"
#include "myguiwidget.h"

namespace Engine
{
	namespace GUI
	{

		class MyGUIButton : public Button, public MyGUIWidget
		{
		public:
			MyGUIButton(const std::string &name, MyGUIWidget *parent);
			MyGUIButton(const std::string &name, MyGUITopLevelWindow &window);

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