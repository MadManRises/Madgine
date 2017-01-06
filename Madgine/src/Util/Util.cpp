#include "madginelib.h"

#include "Util.h"
#include "UtilMethods.h"

namespace Engine {
	namespace Util {

		Util::Util() :
			TopLevelSerializableUnit(Serialize::UTIL),
			mProfiler(this)
		{
			UtilMethods::setup();
		}

		Profiler * Util::profiler()
		{
			return mProfiler.ptr();
		}

	}
}