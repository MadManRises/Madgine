#include "../ogrelib.h"

#include "myguibar.h"

#include "myguitoplevelwindow.h"

namespace Engine
{
	namespace GUI
	{
		MyGUIBar::MyGUIBar(const std::string& name, MyGUIWidget* parent) :
			Widget(name, parent),
		Bar(name, parent),
			MyGUIWidget(name, parent)
		{
			mBar = parent->widget()->createWidget<MyGUI::ProgressBar>("LoadingScreenProgressBar", 0, 0, 0, 0, MyGUI::Align::Default, name);
			mBar->setProgressRange(10000);
		}

		MyGUIBar::MyGUIBar(const std::string& name, MyGUITopLevelWindow& window) :
			Widget(name, window),
		Bar(name, window),
			MyGUIWidget(name, window)
		{
			mBar = window.rootWidget()->createWidget<MyGUI::ProgressBar>("LoadginScreenProgressBar", 0, 0, 0, 0, MyGUI::Align::Default, name);
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