#pragma once

#include "../serialize/serializableunit.h"
#include "../serialize/container/map.h"
#include "processstats.h"


namespace Engine
{
	namespace Util
	{

		class MADGINE_BASE_EXPORT Profiler : public Serialize::SerializableUnit<Profiler>, public Singleton<Profiler>
		{
		public:
			Profiler();
			Profiler(const Profiler&) = delete;

			void startProfiling(const std::string& name);
			void stopProfiling();

			void update();


		private:
			ProcessStats& getProcess(const std::string& name);

			std::tuple<std::function<bool()>> createProcessData();

			Serialize::ObservableMap<std::string, ProcessStats, Serialize::ContainerPolicies::masterOnly, Serialize::ParentCreator<
				                         &Profiler::createProcessData>> mProcesses;

			ProcessStats* mCurrent;

			std::chrono::time_point<std::chrono::high_resolution_clock> mLast;
			float mInterval;
			bool mCurrentInterval;
		};

	}
}
