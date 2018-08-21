#include "../ogrelib.h"

#include "myguibutton.h"

#include "myguitoplevelwindow.h"

namespace Engine
{
	namespace GUI
	{
		MyGUIButton::MyGUIButton(const std::string& name, MyGUIWidget* parent) :
			Widget(name, parent),
			Button(name, parent),
			MyGUIWidget(name, parent)
		{
			mButton = parent->widget()->createWidget<MyGUI::Button>("Button", 0, 0, 0, 0, MyGUI::Align::Default, name);
			mButton->eventMouseButtonClick += MyGUI::newDelegate(this, &MyGUIButton::clicked);
		}

		MyGUIButton::MyGUIButton(const std::string& name, MyGUITopLevelWindow& window) :
			Widget(name, window),
			Button(name, window),
			MyGUIWidget(name, window)
		{
			mButton = window.rootWidget()->createWidget<MyGUI::Button>("Button", 0, 0, 0, 0, MyGUI::Align::Default, name);
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