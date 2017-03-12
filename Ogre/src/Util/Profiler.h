#pragma once

#include "Serialize\serializableunit.h"
#include "Serialize\Container\map.h"
#include "Serialize\Container\observed.h"

namespace Engine {
	namespace Util {

		class OGREMADGINE_EXPORT ProcessStats : public Engine::Serialize::SerializableUnitBase {
		public:
			ProcessStats(ProcessStats *parent = 0) :
				mStarted(false),
				mAccumulatedDuration(0),
				mRecordIndex(0),
				mBuffer(),
				mChildren(),
				mParent(parent)
			{
			}

			ProcessStats(const ProcessStats &other) :
				ProcessStats(other.mParent)
			{}

			size_t averageDuration() const;
			void start();
			void stop();

			ProcessStats &addChild(const std::string &child);

			bool hasParent() const;
			const ProcessStats *parent() const;
			ProcessStats *parent();

		private:
			std::chrono::time_point<std::chrono::high_resolution_clock> mStart;

			bool mStarted;

			Serialize::Observed<size_t> mAccumulatedDuration;
			size_t mRecordIndex;
			std::array<size_t, 20> mBuffer;

			Serialize::ObservableMap<std::string, ProcessStats, Serialize::ContainerPolicy::masterOnly, Serialize::DefaultCreator<std::string, ProcessStats*>> mChildren;

			ProcessStats *mParent;

		};


		class OGREMADGINE_EXPORT Profiler : public Serialize::SerializableUnit<Profiler> {
		public:
			Profiler();

			void startProfiling(const std::string &name);
			void stopProfiling();

			void update();


		private:
			ProcessStats &getProcess(const std::string &name);

			Serialize::ObservableMap<std::string, ProcessStats, Serialize::ContainerPolicy::masterOnly> mProcesses;
			
			ProcessStats *mCurrent;

			std::chrono::time_point<std::chrono::high_resolution_clock> mLast;
			float mInterval;
			bool mCurrentInterval;
		};

		class OGREMADGINE_EXPORT ProfileWrapper {
		public:
			ProfileWrapper(const std::string &name);
			~ProfileWrapper();
		};

#define PROFILE(TARGET) Engine::Util::ProfileWrapper __p(TARGET)

	}
}