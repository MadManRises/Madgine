#include "libinclude.h"

#include "MyGUITextbox.h"

#include "MyGUIWindow.h"

#pragma warning (push, 0)
#include <MYGUI\MyGUI.h>
#pragma warning (pop)

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