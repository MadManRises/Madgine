#include "libinclude.h"

#include "MyGUICheckbox.h"
#include "MyGUIWindow.h"

#pragma warning (push, 0)
#include <MYGUI\MyGUI.h>
#pragma warning (pop)

namespace Engine {
	namespace GUI {
		namespace MyGui {
			MyGUICheckbox::MyGUICheckbox(MyGUIWindow *w) :
				Checkbox(w),
				mCheckbox(static_cast<MyGUI::Button*>(w->window()))
			{
				mCheckbox->eventMouseButtonClick += MyGUI::newDelegate(this, &MyGUICheckbox::toggle);
			}


			bool MyGUICheckbox::isChecked()
			{
				return mCheckbox->getStateCheck();
			}

			void MyGUICheckbox::setChecked(bool b)
			{
				mCheckbox->setStateSelected(b);
			}
			void MyGUICheckbox::toggle(MyGUI::Widget * t)
			{
				mCheckbox->setStateSelected(!mCheckbox->getStateSelected());
			}
		}
	}
}