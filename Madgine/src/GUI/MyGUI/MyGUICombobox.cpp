#include "libinclude.h"

#include "MyGUICombobox.h"
#include "MyGUIWindow.h"


#ifdef _MSC_VER
#pragma warning (push, 0)
#endif
#include <MYGUI\MyGUI.h>
#ifdef _MSC_VER
#pragma warning (pop)
#endif


namespace Engine {
	namespace GUI {
		namespace MyGui {

			MyGUICombobox::MyGUICombobox(MyGUIWindow *w) :
				Combobox(w),
				mCombobox(w->window()->castType<MyGUI::ComboBox>())
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