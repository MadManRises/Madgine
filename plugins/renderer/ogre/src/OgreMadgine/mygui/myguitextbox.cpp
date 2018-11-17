#include "../ogrelib.h"

#include "myguitextbox.h"

#include "myguitoplevelwindow.h"

namespace Engine
{
	namespace GUI
	{
		MyGUITextbox::MyGUITextbox(const std::string& name, MyGUIWidget* parent) :
		MyGUIWidget(name, parent),
			Widget(name, parent),
		Textbox(name, parent)
		{
			mTextbox = parent->widget()->createWidget<MyGUI::EditBox>("Textbox", 0, 0, 0, 0, MyGUI::Align::Default, name);
		}

		MyGUITextbox::MyGUITextbox(const std::string& name, MyGUITopLevelWindow& window) :
		MyGUIWidget(name, window),
			Widget(name, window),
		Textbox(name, window)
		{
			mTextbox = window.rootWidget()->createWidget<MyGUI::EditBox>("Textbox", 0, 0, 0, 0, MyGUI::Align::Default, name);
		}

		std::string MyGUITextbox::getText()
		{
			return mTextbox->getCaption().asUTF8();
		}

		void MyGUITextbox::setText(const std::string& text)
		{
			mTextbox->setCaption(text);
			mTextbox->eventEditTextChange(mTextbox);
		}

		void MyGUITextbox::setEditable(bool b)
		{
			mTextbox->setEditReadOnly(!b);
		}

		Class MyGUITextbox::getClass()
		{
			return Class::TEXTBOX_CLASS;
		}

		MyGUI::Widget* MyGUITextbox::widget() const
		{
			return mTextbox;
		}
	}
}