#include "../maditorlauncherlib.h"

#include "appstats.h"

#include "Madgine/app/application.h"

namespace Maditor {
	namespace Launcher {

		AppStats::AppStats()
		{
		}



		void AppStats::setApp(Engine::App::Application * app)
		{
			mApp = app;
		}

		void AppStats::update()
		{
			mAverageFPS = mApp ? mApp->getFPS() : 0.0f;
		}
		
	}
}
