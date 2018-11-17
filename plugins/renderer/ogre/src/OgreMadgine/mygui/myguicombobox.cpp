#include "../ogrelib.h"

#include "myguicombobox.h"

#include "myguitoplevelwindow.h"

namespace Engine
{
	namespace GUI
	{
		MyGUICombobox::MyGUICombobox(const std::string& name, MyGUIWidget* parent) :
			Widget(name, parent),
			Combobox(name, parent),
			MyGUIWidget(name, parent)
		{
			mCombobox = parent->widget()->createWidget<MyGUI::ComboBox>("ComboBox", 0, 0, 0, 0, MyGUI::Align::Default, name);
			mCombobox->setEditReadOnly(true);
		}

		MyGUICombobox::MyGUICombobox(const std::string& name, MyGUITopLevelWindow& window) :
			Widget(name, window),
		Combobox(name, window),
			MyGUIWidget(name, window)
		{
			mCombobox = window.rootWidget()->createWidget<MyGUI::ComboBox>("ComboBox", 0, 0, 0, 0, MyGUI::Align::Default, name);
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