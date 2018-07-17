#include "../ogrelib.h"

#include "myguitabwindow.h"

#include "myguisystem.h"

namespace Engine
{
	namespace GUI
	{
		MyGUITabWindow::MyGUITabWindow(const std::string& name, MyGUIWindow* parent) :
		MyGUIWindow(name, parent),
		Window(name, parent),
		TabWindow(name, parent)
		{
			mWidget = parent->widget()->createWidget<MyGUI::Widget>("PanelEmpty", 0, 0, 0, 0, MyGUI::Align::Default, name);
		}

		MyGUITabWindow::MyGUITabWindow(const std::string& name, MyGUISystem& system) :
		MyGUIWindow(name, system),
		Window(name, system),
		TabWindow(name, system)
		{
			mWidget = system.rootWidget()->createWidget<MyGUI::Widget>("PanelEmpty", 0, 0, 0, 0, MyGUI::Align::Default, name);
		}

		Class MyGUITabWindow::getClass()
		{
			return Class::TABWINDOW_CLASS;
		}

		void MyGUITabWindow::addTab(Window* w)
		{
		}

		MyGUI::Widget* MyGUITabWindow::widget() const
		{
			return mWidget;
		}
	}
}