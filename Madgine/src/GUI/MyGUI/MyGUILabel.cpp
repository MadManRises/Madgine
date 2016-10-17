#include "libinclude.h"

#include "MyGUILabel.h"

#include "MyGUIWindow.h"

#pragma warning (push, 0)
#include <MYGUI\MyGUI.h>
#pragma warning (pop)

namespace Engine {
	namespace GUI {
		namespace MyGui {
			MyGUILabel::MyGUILabel(MyGUIWindow * w) :
				Label(w),
				mLabel(static_cast<MyGUI::TextBox*>(w->window()))
			{
			}
			void MyGUILabel::setText(const std::string & text)
			{
				mLabel->setCaption(text);
			}
		}
	}
}
