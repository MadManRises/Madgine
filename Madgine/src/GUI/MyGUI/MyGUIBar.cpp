#include "libinclude.h"

#include "MyGUIBar.h"
#include "MyGUIWindow.h"

#pragma warning (push, 0)
#include <MYGUI\MyGUI.h>
#pragma warning (pop)

namespace Engine {
	namespace GUI {
		namespace MyGui {
			MyGUIBar::MyGUIBar(MyGUIWindow *w) :
				Bar(w),
				mBar(static_cast<MyGUI::ProgressBar*>(w->window()))
			{
				mBar->setProgressRange(100);
			}

			void MyGUIBar::setRatio(float f)
			{
				mBar->setProgressPosition(f * 100.f);
			}
		}
	}
}