#include "../maditorlauncherlib.h"

#include "util.h"

namespace Maditor {
	namespace Launcher {

		Util::Util()
		{
		}

		void Util::setApp(Engine::App::Application * app)
		{
			mStats->setApp(app);
		}

		Engine::Util::Profiler * Util::profiler()
		{
			return mProfiler.ptr();
		}

		void Util::update()
		{
			mProfiler->update();
			mStats->update();
		}

	}
}
