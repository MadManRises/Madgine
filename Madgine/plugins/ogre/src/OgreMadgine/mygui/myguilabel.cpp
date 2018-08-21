#include "../ogrelib.h"

#include "myguilabel.h"

#include "myguitoplevelwindow.h"

namespace Engine
{
	namespace GUI
	{
		MyGUILabel::MyGUILabel(const std::string& name, MyGUIWidget* parent) :
		MyGUIWidget(name, parent),
			Widget(name, parent),
		Label(name, parent)
		{
			mTextBox = parent->widget()->createWidget<MyGUI::TextBox>("Textbox", 0, 0, 0, 0, MyGUI::Align::Default, name);
		}

		MyGUILabel::MyGUILabel(const std::string& name, MyGUITopLevelWindow& window) :
			Widget(name, window),
		MyGUIWidget(name, window),
		Label(name, window)
		{
			mTextBox = window.rootWidget()->createWidget<MyGUI::TextBox>("Textbox", 0, 0, 0, 0, MyGUI::Align::Default, name);
		}

		void MyGUILabel::setText(const std::string& text)
		{
			mTextBox->setCaption(text);
		}

		Class MyGUILabel::getClass()
		{
			return Class::LABEL_CLASS;
		}

		MyGUI::Widget* MyGUILabel::widget() const
		{
			return mTextBox;
		}
	}
}