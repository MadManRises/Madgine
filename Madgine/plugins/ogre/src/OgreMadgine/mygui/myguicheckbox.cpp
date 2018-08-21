#include "../ogrelib.h"

#include "myguicheckbox.h"

#include "myguitoplevelwindow.h"

namespace Engine
{
	namespace GUI
	{
		MyGUICheckbox::MyGUICheckbox(const std::string& name, MyGUIWidget* parent) :
			Widget(name, parent),
		Checkbox(name, parent),
			MyGUIWidget(name, parent)
		{
			 mCheckbox = parent->widget()->createWidget<MyGUI::Button>("Checkbox", 0, 0, 0, 0, MyGUI::Align::Default, name);
			 mCheckbox->eventMouseButtonClick += MyGUI::newDelegate(this, &MyGUICheckbox::toggle);
		}

		MyGUICheckbox::MyGUICheckbox(const std::string& name, MyGUITopLevelWindow& window) :
			Widget(name, window),
		Checkbox(name, window),
			MyGUIWidget(name, window)
		{
			mCheckbox = window.rootWidget()->createWidget<MyGUI::Button>("Checkbox", 0, 0, 0, 0, MyGUI::Align::Default, name);
			mCheckbox->eventMouseButtonClick += MyGUI::newDelegate(this, &MyGUICheckbox::toggle);
		}

		bool MyGUICheckbox::isChecked()
		{
			return mCheckbox->getStateSelected();
		}

		void MyGUICheckbox::setChecked(bool b)
		{
			mCheckbox->setStateSelected(b);
		}

		Class MyGUICheckbox::getClass()
		{
			return Class::CHECKBOX_CLASS;
		}

		void MyGUICheckbox::toggle(MyGUI::Widget* t)
		{
			mCheckbox->setStateSelected(!mCheckbox->getStateSelected());
		}

		MyGUI::Widget* MyGUICheckbox::widget() const
		{
			return mCheckbox;
		}
	}
}