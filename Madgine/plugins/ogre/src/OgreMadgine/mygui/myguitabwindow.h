#pragma once

#include "Madgine/gui/windows/tabwindow.h"
#include "myguiwindow.h"

namespace Engine
{
	namespace GUI
	{

		class MyGUITabWindow : public TabWindow, public MyGUIWindow
		{
		public:
			MyGUITabWindow(const std::string &name, MyGUIWindow *parent);
			MyGUITabWindow(const std::string &name, MyGUISystem &system);

			Class getClass() override;

			void addTab(Window* w) override;

		protected:
			MyGUI::Widget* widget() const override;

		private:
			MyGUI::Widget *mWidget;
		};

	}
}