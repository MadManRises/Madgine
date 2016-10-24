#include "libinclude.h"

#include "MyGUITextbox.h"

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
			MyGUITextbox::MyGUITextbox(MyGUIWindow * w) :
				Textbox(w),
				mTextbox(static_cast<MyGUI::EditBox*>(w->window()))
			{
			}
			std::string MyGUITextbox::getText()
			{
				return mTextbox->getCaption().asUTF8();
			}
			void MyGUITextbox::setText(const std::string & text)
			{
				mTextbox->setCaption(text);
				mTextbox->eventEditTextChange(mTextbox);
			}
			void MyGUITextbox::setEditable(bool b)
			{
				mTextbox->setEditReadOnly(!b);
			}
		}
	}
}