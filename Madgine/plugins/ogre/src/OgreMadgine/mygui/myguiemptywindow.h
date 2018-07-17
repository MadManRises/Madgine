#pragma once

#include "myguiwindow.h"

namespace Engine
{
	namespace GUI
	{

		class MyGUIEmptyWindow : public MyGUIWindow
		{
		public:
			MyGUIEmptyWindow(const std::string &name, MyGUIWindow *parent);
			MyGUIEmptyWindow(const std::string &name, MyGUISystem &system);

			Class getClass() override;

		protected:
			MyGUI::Widget* widget() const override;

		private:
			MyGUI::Widget *mWidget;
		};

	}
}