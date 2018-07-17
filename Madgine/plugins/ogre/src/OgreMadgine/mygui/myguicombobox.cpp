#include "../ogrelib.h"

#include "myguicombobox.h"

#include "myguisystem.h"

namespace Engine
{
	namespace GUI
	{
		MyGUICombobox::MyGUICombobox(const std::string& name, MyGUIWindow* parent) :
			MyGUIWindow(name, parent),
			Window(name, parent),
			Combobox(name, parent)
		{
			mCombobox = parent->widget()->createWidget<MyGUI::ComboBox>("ComboBox", 0, 0, 0, 0, MyGUI::Align::Default, name);
			mCombobox->setEditReadOnly(true);
		}

		MyGUICombobox::MyGUICombobox(const std::string& name, MyGUISystem& system) :
		Window(name, system),
		MyGUIWindow(name, system),
		Combobox(name, system)
		{
			mCombobox = system.rootWidget()->createWidget<MyGUI::ComboBox>("ComboBox", 0, 0, 0, 0, MyGUI::Align::Default, name);
			mCombobox->setEditReadOnly(true);
		}

		void MyGUICombobox::addItem(const std::string& text)
		{
			mCombobox->addItem(text);
		}

		void MyGUICombobox::clear()
		{
			mCombobox->removeAllItems();
		}

		void MyGUICombobox::setText(const std::string& s)
		{
			mCombobox->setCaption(s);
		}

		std::string MyGUICombobox::getText()
		{
			return mCombobox->getCaption();
		}

		Class MyGUICombobox::getClass()
		{
			return Class::COMBOBOX_CLASS;
		}

		MyGUI::Widget* MyGUICombobox::widget() const
		{
			return mCombobox;
		}
	}
}