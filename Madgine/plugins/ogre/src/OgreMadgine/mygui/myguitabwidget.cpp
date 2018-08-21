#include "../ogrelib.h"

#include "myguitabwidget.h"

#include "myguitoplevelwindow.h"

namespace Engine
{
	namespace GUI
	{
		MyGUITabWidget::MyGUITabWidget(const std::string& name, MyGUIWidget* parent) :
		MyGUIWidget(name, parent),
		Widget(name, parent),
		TabWidget(name, parent)
		{
			mWidget = parent->widget()->createWidget<MyGUI::Widget>("PanelEmpty", 0, 0, 0, 0, MyGUI::Align::Default, name);
		}

		MyGUITabWidget::MyGUITabWidget(const std::string& name, MyGUITopLevelWindow& window) :
		MyGUIWidget(name, window),
			Widget(name, window),
		TabWidget(name, window)
		{
			mWidget = window.rootWidget()->createWidget<MyGUI::Widget>("PanelEmpty", 0, 0, 0, 0, MyGUI::Align::Default, name);
		}

		Class MyGUITabWidget::getClass()
		{
			return Class::TABWINDOW_CLASS;
		}

		void MyGUITabWidget::addTab(Widget* w)
		{
		}

		MyGUI::Widget* MyGUITabWidget::widget() const
		{
			return mWidget;
		}
	}
}