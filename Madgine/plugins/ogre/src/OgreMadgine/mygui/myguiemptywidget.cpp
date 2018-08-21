#include "../ogrelib.h"

#include "myguiemptywidget.h"

#include "myguitoplevelwindow.h"

namespace Engine
{
	namespace GUI
	{
		MyGUIEmptyWidget::MyGUIEmptyWidget(const std::string& name, MyGUIWidget* parent) :
		MyGUIWidget(name, parent),
			Widget(name, parent)
		{
			mWidget = parent->widget()->createWidget<MyGUI::Widget>("PanelEmpty", 0, 0, 0, 0, MyGUI::Align::Default, name);
		}

		MyGUIEmptyWidget::MyGUIEmptyWidget(const std::string& name, MyGUITopLevelWindow& window) :
		MyGUIWidget(name, window),
			Widget(name, window)
		{
			mWidget = window.rootWidget()->createWidget<MyGUI::Widget>("PanelEmpty", 0, 0, 0, 0, MyGUI::Align::Default, name);
		}

		Class MyGUIEmptyWidget::getClass()
		{
			return Class::WINDOW_CLASS;
		}

		MyGUI::Widget* MyGUIEmptyWidget::widget() const
		{
			return mWidget;
		}
	}
}