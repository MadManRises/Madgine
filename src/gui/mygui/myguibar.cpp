#include "../../ogrelib.h"

#include "myguibar.h"
#include "myguiwindow.h"


namespace Engine
{
	namespace GUI
	{
		namespace MyGui
		{
			MyGUIBar::MyGUIBar(MyGUIWindow* w) :
				Bar(w),
				mBar(w->window()->castType<MyGUI::ProgressBar>())
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
