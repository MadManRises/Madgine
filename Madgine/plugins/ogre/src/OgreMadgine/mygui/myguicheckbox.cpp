#include "../ogrelib.h"

#include "myguicheckbox.h"

#include "myguisystem.h"

namespace Engine
{
	namespace GUI
	{
		MyGUICheckbox::MyGUICheckbox(const std::string& name, MyGUIWindow* parent) :
		MyGUIWindow(name, parent),
		Window(name, parent),
		Checkbox(name, parent)
		{
			 mCheckbox = parent->widget()->createWidget<MyGUI::Button>("Checkbox", 0, 0, 0, 0, MyGUI::Align::Default, name);
			 mCheckbox->eventMouseButtonClick += MyGUI::newDelegate(this, &MyGUICheckbox::toggle);
		}

		MyGUICheckbox::MyGUICheckbox(const std::string& name, MyGUISystem& system) :
		Window(name, system),
		MyGUIWindow(name, system),
		Checkbox(name, system)
		{
			mCheckbox = system.rootWidget()->createWidget<MyGUI::Button>("Checkbox", 0, 0, 0, 0, MyGUI::Align::Default, name);
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