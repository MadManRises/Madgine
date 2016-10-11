#include "libinclude.h"

#include "MyGUICombobox.h"
#include "MyGUIWindow.h"

#pragma warning (push, 0)
#include <MYGUI\MyGUI.h>
#pragma warning (pop)

namespace Engine {
	namespace GUI {
		namespace MyGui {

			MyGUICombobox::MyGUICombobox(MyGUIWindow *w) :
				Combobox(w),
				mCombobox(static_cast<MyGUI::ComboBox*>(w->window()))
			{
				mCombobox->setEditReadOnly(true);
			}

			void MyGUICombobox::addItem(const std::string & text)
			{
				mCombobox->addItem(text);
			}

			void MyGUICombobox::clear()
			{
				mCombobox->removeAllItems();
			}

			void MyGUICombobox::setText(const std::string & s)
			{
				mCombobox->setCaption(s);
			}

			std::string MyGUICombobox::getText()
			{
				return mCombobox->getCaption();
			}

		}
	}
}