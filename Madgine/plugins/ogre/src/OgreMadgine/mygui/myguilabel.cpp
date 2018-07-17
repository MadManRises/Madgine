#include "../ogrelib.h"

#include "myguilabel.h"

#include "myguisystem.h"

namespace Engine
{
	namespace GUI
	{
		MyGUILabel::MyGUILabel(const std::string& name, MyGUIWindow* parent) :
		MyGUIWindow(name, parent),
		Window(name, parent),
		Label(name, parent)
		{
			mTextBox = parent->widget()->createWidget<MyGUI::TextBox>("Textbox", 0, 0, 0, 0, MyGUI::Align::Default, name);
		}

		MyGUILabel::MyGUILabel(const std::string& name, MyGUISystem& system) :
		Window(name, system),
		MyGUIWindow(name, system),
		Label(name, system)
		{
			mTextBox = system.rootWidget()->createWidget<MyGUI::TextBox>("Textbox", 0, 0, 0, 0, MyGUI::Align::Default, name);
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