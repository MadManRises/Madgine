#include "../ogrelib.h"

#include "myguibar.h"

#include "myguisystem.h"

namespace Engine
{
	namespace GUI
	{
		MyGUIBar::MyGUIBar(const std::string& name, MyGUIWindow* parent) :
		Window(name, parent),
		MyGUIWindow(name, parent),		
		Bar(name, parent)
		{
			mBar = parent->widget()->createWidget<MyGUI::ProgressBar>("LoadingScreenProgressBar", 0, 0, 0, 0, MyGUI::Align::Default, name);
			mBar->setProgressRange(10000);
		}

		MyGUIBar::MyGUIBar(const std::string& name, MyGUISystem& system) :
		Window(name, system),
		MyGUIWindow(name, system),		
		Bar(name, system)
		{
			mBar = system.rootWidget()->createWidget<MyGUI::ProgressBar>("LoadginScreenProgressBar", 0, 0, 0, 0, MyGUI::Align::Default, name);
			mBar->setProgressRange(10000);
		}

		void MyGUIBar::setRatio(float f)
		{
			return mBar->setProgressPosition(10000 * f);
		}

		Class MyGUIBar::getClass()
		{
			return Class::BAR_CLASS;
		}

		MyGUI::Widget* MyGUIBar::widget() const
		{
			return mBar;
		}
	}
}