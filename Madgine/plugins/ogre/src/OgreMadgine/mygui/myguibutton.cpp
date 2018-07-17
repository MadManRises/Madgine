#include "../ogrelib.h"

#include "myguibutton.h"

#include "myguisystem.h"

namespace Engine
{
	namespace GUI
	{
		MyGUIButton::MyGUIButton(const std::string& name, MyGUIWindow* parent) :
			MyGUIWindow(name, parent),
			Window(name, parent),
			Button(name, parent)
		{
			mButton = parent->widget()->createWidget<MyGUI::Button>("Button", 0, 0, 0, 0, MyGUI::Align::Default, name);
			mButton->eventMouseButtonClick += MyGUI::newDelegate(this, &MyGUIButton::clicked);
		}

		MyGUIButton::MyGUIButton(const std::string& name, MyGUISystem& system) :
			Window(name, system),
			MyGUIWindow(name, system),
			Button(name, system)
		{
			mButton = system.rootWidget()->createWidget<MyGUI::Button>("Button", 0, 0, 0, 0, MyGUI::Align::Default, name);
		}

		void MyGUIButton::setText(const std::string& text)
		{
			mButton->setCaption(text);
		}

		Class MyGUIButton::getClass()
		{
			return Class::BUTTON_CLASS;
		}

		MyGUI::Widget* MyGUIButton::widget() const
		{
			return mButton;
		}

		void MyGUIButton::clicked(MyGUI::Widget* sender)
		{
			emitClicked();
		}
	}
}