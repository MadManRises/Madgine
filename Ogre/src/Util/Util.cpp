#include "madginelib.h"

#include "Util.h"
#include "Util/UtilMethods.h"

#include "TopLevelIds.h"

namespace Engine {
	namespace Util {

		Util::Util(Ogre::RenderWindow *window) :
			TopLevelSerializableUnit(UTIL),
			mLog("Madgine.log"),
			mStats(window)
		{
			UtilMethods::setup(&mLog);
		}

		Profiler * Util::profiler()
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