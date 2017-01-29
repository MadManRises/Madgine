#include "madginelib.h"

#include "Util.h"
#include "UtilMethods.h"

namespace Engine {
	namespace Util {

		Util::Util(Ogre::RenderWindow *window) :
			TopLevelSerializableUnit(Serialize::UTIL),
			mProfiler(this),
			mStats(this, window)
		{
			UtilMethods::setup();
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