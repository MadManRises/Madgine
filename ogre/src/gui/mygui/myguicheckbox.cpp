#include "ogrelib.h"

#include "myguicheckbox.h"
#include "myguiwindow.h"


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
			MyGUICheckbox::MyGUICheckbox(MyGUIWindow *w) :
				Checkbox(w),
				mCheckbox(w->window()->castType<MyGUI::Button>())
			{
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
			void MyGUICheckbox::toggle(MyGUI::Widget * t)
			{
				mCheckbox->setStateSelected(!mCheckbox->getStateSelected());
			}
		}
	}
}