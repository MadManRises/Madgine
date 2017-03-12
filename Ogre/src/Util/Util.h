#pragma once

#include "Profiler.h"
#include "AppStats.h"

#include "Serialize\toplevelserializableunit.h"

#include "Serialize\Container\serialized.h"

#include "OgreUtilLog.h"

namespace Engine {
	namespace Util {

		class OGREMADGINE_EXPORT Util : public Singleton<Util>, public Serialize::TopLevelSerializableUnit<Util> {
		public:
			Util(Ogre::RenderWindow *window);

			Profiler *profiler();

			void update();

		private:
			OgreUtilLog mLog;

			Serialize::Serialized<Profiler> mProfiler;
			Serialize::Serialized<AppStats> mStats;

		};

	}
}