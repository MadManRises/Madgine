#include "libinclude.h"

#include "MyGUIBar.h"
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