#pragma once

#include "Madgine/serialize/serializableunit.h"

#include "Madgine/serialize/container/serialized.h"

#include "profilermodel.h"
#include "statsmodel.h"

namespace Maditor {
	namespace Model {

		class MADITOR_MODEL_EXPORT UtilModel : public Engine::Serialize::SerializableUnit<UtilModel> {
		public:

			UtilModel();
			UtilModel(const UtilModel &) = delete;

			ProfilerModel *profiler();
			StatsModel *stats();

			void reset();

		private:
			Engine::Serialize::Serialized<ProfilerModel> mProfiler;
			Engine::Serialize::Serialized<StatsModel> mStats;

		};
	}
}
