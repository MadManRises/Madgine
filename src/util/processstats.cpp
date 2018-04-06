#include "../baselib.h"

#include "processstats.h"


namespace Engine
{

	namespace Util
	{

		void ProcessStats::start()
		{
			assert(!mStarted);
			mStarted = true;
			mStart = std::chrono::high_resolution_clock::now();
		}

		void ProcessStats::stop()
		{
			assert(mStarted);
			mStarted = false;
			auto end = std::chrono::high_resolution_clock::now();
			size_t duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - mStart).count();

			//mAccumulatedDuration += duration - mBuffer[mRecordIndex];
			mBuffer[mRecordIndex] = duration;
			++mRecordIndex;
			mRecordIndex %= 20;
		}

		ProcessStats& ProcessStats::addChild(const std::string& child)
		{
			return mChildren.try_emplace(child, this).first->second;
		}

		bool ProcessStats::hasParent() const
		{
			return mParent != nullptr;
		}

		const ProcessStats* ProcessStats::parent() const
		{
			return mParent;
		}

		ProcessStats* ProcessStats::parent()
		{
			return mParent;
		}
	}
}
