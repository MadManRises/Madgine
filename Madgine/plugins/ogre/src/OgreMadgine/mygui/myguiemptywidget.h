#pragma once

#include "myguiwidget.h"

namespace Engine
{
	namespace GUI
	{

		class MyGUIEmptyWidget : public MyGUIWidget
		{
		public:
			MyGUIEmptyWidget(const std::string &name, MyGUIWidget *parent);
			MyGUIEmptyWidget(const std::string &name, MyGUITopLevelWindow &system);

			Class getClass() override;

		protected:
			MyGUI::Widget* widget() const override;

		private:
			MyGUI::Widget *mWidget;
		};

	}
}