#pragma once

#include "Madgine/gui/widgets/tabwidget.h"
#include "myguiwidget.h"

namespace Engine
{
	namespace GUI
	{

		class MyGUITabWidget : public TabWidget, public MyGUIWidget
		{
		public:
			MyGUITabWidget(const std::string &name, MyGUIWidget *parent);
			MyGUITabWidget(const std::string &name, MyGUITopLevelWindow &window);

			Class getClass() override;

			void addTab(Widget* w) override;

		protected:
			MyGUI::Widget* widget() const override;

		private:
			MyGUI::Widget *mWidget;
		};

	}
}