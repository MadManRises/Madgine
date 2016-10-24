#include "libinclude.h"

#include "MyGUILabel.h"

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
