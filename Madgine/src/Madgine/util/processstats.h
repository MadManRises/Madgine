#pragma once

#include "../serialize/serializableunit.h"
#include "../serialize/container/map.h"
#include "../serialize/container/observed.h"

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

			Serialize::ObservableMap<std::string, ProcessStats, Serialize::ContainerPolicies::masterOnly, Serialize::DefaultCreator
			                         <ProcessStats*>> mChildren;

			ProcessStats* mParent;
		};

	}
}
