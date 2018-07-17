#include "../ogrelib.h"

#include "myguiemptywindow.h"

#include "myguisystem.h"

namespace Engine
{
	namespace GUI
	{
		MyGUIEmptyWindow::MyGUIEmptyWindow(const std::string& name, MyGUIWindow* parent) :
		MyGUIWindow(name, parent),
		Window(name, parent)
		{
			mWidget = parent->widget()->createWidget<MyGUI::Widget>("PanelEmpty", 0, 0, 0, 0, MyGUI::Align::Default, name);
		}

		MyGUIEmptyWindow::MyGUIEmptyWindow(const std::string& name, MyGUISystem& system) :
		MyGUIWindow(name, system),
		Window(name, system)
		{
			mWidget = system.rootWidget()->createWidget<MyGUI::Widget>("PanelEmpty", 0, 0, 0, 0, MyGUI::Align::Default, name);
		}

		Class MyGUIEmptyWindow::getClass()
		{
			return Class::WINDOW_CLASS;
		}

		MyGUI::Widget* MyGUIEmptyWindow::widget() const
		{
			return mWidget;
		}
	}
}