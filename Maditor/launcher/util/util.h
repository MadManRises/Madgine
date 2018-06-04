#pragma once

#include "Madgine/util/profiler.h"
#include "appstats.h"

#include "Madgine/serialize/toplevelserializableunit.h"

#include "Madgine/serialize/container/serialized.h"


namespace Maditor {
	namespace Launcher {

		class Util : public Engine::Serialize::SerializableUnit<Util> {
		public:
			Util();

			void setApp(Engine::App::Application* app);
			
			Engine::Util::Profiler *profiler();

			void update();

		private:

			Engine::Serialize::Serialized<Engine::Util::Profiler> mProfiler;
			Engine::Serialize::Serialized<AppStats> mStats;

		};

	}
}
