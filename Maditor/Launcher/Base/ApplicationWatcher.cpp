#include "libinclude.h"

#include "ApplicationWatcher.h"

#include "ApplicationInfo.h"


			ApplicationWatcher::ApplicationWatcher() :
				mGuiRenderWindow(0),
				mSceneRenderWindow(0)
			{
			}


			void ApplicationWatcher::clear()
			{
				mObjectsWatcher.update();
				mPerformanceWatcher.clear();
			}

			void ApplicationWatcher::update()
			{

				mPerformanceWatcher.update();
				mObjectsWatcher.update();

			}


			

