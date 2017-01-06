#pragma once

#include "Profiler.h"

#include "Serialize\toplevelserializableunit.h"

#include "Serialize\Container\serialized.h"

namespace Engine {
	namespace Util {

		class MADGINE_EXPORT Util : public Ogre::Singleton<Util>, public Ogre::GeneralAllocatedObject, public Serialize::TopLevelSerializableUnit {
		public:
			Util();

			Profiler *profiler();

		private:
			Serialize::Serialized<Profiler> mProfiler;

		};

	}
}