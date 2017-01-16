#pragma once

#include "Serialize\serializableunit.h"
#include "Serialize\Container\map.h"
#include "Serialize\Container\observed.h"

namespace Engine {
	namespace Util {

		class MADGINE_EXPORT ProcessStats : public Engine::Serialize::SerializableUnit {
		public:
			ProcessStats(std::function<bool()> condition, ProcessStats *parent = 0) :
				mStarted(false),
				mAccumulatedDuration(this, 0),
				mRecordIndex(0),
				mBuffer(),
				mParent(parent),
				mChildren(this)
			{
				mAccumulatedDuration.setCondition(condition);
			}

			ProcessStats(const ProcessStats &other) :
				ProcessStats(other.mAccumulatedDuration.getCondition(), other.mParent)
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

			Serialize::ObservableMap<ProcessStats, Serialize::ContainerPolicy::masterOnly, std::function<bool()>, ProcessStats*> mChildren;

			ProcessStats *mParent;

		};


		class MADGINE_EXPORT Profiler : public Serialize::SerializableUnit {
		public:
			Profiler();

			void startProfiling(const std::string &name);
			void stopProfiling();

			void update();


		private:
			ProcessStats &getProcess(const std::string &name);

			Serialize::ObservableMap<ProcessStats, Serialize::ContainerPolicy::masterOnly, std::function<bool()>> mProcesses;
			
			ProcessStats *mCurrent;

			std::chrono::time_point<std::chrono::high_resolution_clock> mLast;
			float mInterval;
			bool mCurrentInterval;
		};

		class MADGINE_EXPORT ProfileWrapper {
		public:
			ProfileWrapper(const std::string &name);
			~ProfileWrapper();
		};

#define PROFILE(TARGET) Engine::Util::ProfileWrapper __p(TARGET)

	}
}