#pragma once

#include "serialize/serializableunit.h"
#include "serialize/container/map.h"
#include "serialize/container/observed.h"

namespace Engine
{
	namespace Util
	{
		class MADGINE_BASE_EXPORT ProcessStats : public Serialize::SerializableUnit<ProcessStats>
		{
		public:
			ProcessStats(const std::function<bool()>& condition) :
				mStarted(false),
				mAccumulatedDuration(0),
				mRecordIndex(0),
				mBuffer({}),
				mParent(nullptr)
			{
				//mAccumulatedDuration.setCondition(condition);
			}

			ProcessStats(ProcessStats* parent) :
				mStarted(false),
				mAccumulatedDuration(0),
				mRecordIndex(0),
				mBuffer({}),
				mParent(parent)
			{
				//mAccumulatedDuration.setCondition(parent->mAccumulatedDuration.getCondition());
			}

			ProcessStats(const ProcessStats& other) :
				ProcessStats(other.mParent)
			{
			}

			size_t averageDuration() const;
			void start();
			void stop();

			ProcessStats& addChild(const std::string& child);

			bool hasParent() const;
			const ProcessStats* parent() const;
			ProcessStats* parent();

		private:

			std::chrono::time_point<std::chrono::high_resolution_clock> mStart;

			bool mStarted;

			Serialize::Observed<size_t> mAccumulatedDuration;
			size_t mRecordIndex;
			std::array<size_t, 20> mBuffer;

			Serialize::ObservableMap<std::string, ProcessStats, Serialize::ContainerPolicy::masterOnly, Serialize::DefaultCreator
			                         <ProcessStats*>> mChildren;

			ProcessStats* mParent;
		};


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

			Serialize::ObservableMap<std::string, ProcessStats, Serialize::ContainerPolicy::masterOnly, Serialize::ParentCreator<
				                         decltype(&Profiler::createProcessData), &Profiler::createProcessData>> mProcesses;

			ProcessStats* mCurrent;

			std::chrono::time_point<std::chrono::high_resolution_clock> mLast;
			float mInterval;
			bool mCurrentInterval;
		};

		class MADGINE_BASE_EXPORT ProfileWrapper
		{
		public:
			ProfileWrapper(const std::string& name);
			~ProfileWrapper();
		};

#define PROFILE(TARGET) Engine::Util::ProfileWrapper __p(TARGET)
	}
}
