#include "maditormodellib.h"

#include "statsmodel.h"

#ifdef _WIN32
#include <Psapi.h>
#endif

namespace Maditor {
	namespace Model {

		StatsModel::StatsModel() :
			mTimerId(0),
			mCurrentUsage(0)
		{
			mAverageFPS.setCallback([this](const float &f) {emit averageFPSChanged(f); });
			//mOgreMemoryUsage.setCallback([this](const size_t &mem) {emit ogreMemoryUsageChanged(mem); });
		}

		void StatsModel::reset()
		{
			mAverageFPS = 0.0f;
			//mOgreMemoryUsage = 0;
			if (mCurrentUsage) {
				mCurrentUsage = 0;
				emit memoryUsageChanged(0);
			}
			if (mTimerId) {
				killTimer(mTimerId);
				mTimerId = 0;
			}
			mHandle = 0;
		}

		void StatsModel::timerEvent(QTimerEvent * evt)
		{
#ifdef _WIN32
			PROCESS_MEMORY_COUNTERS memCounter;
			BOOL result = GetProcessMemoryInfo(mHandle,
				&memCounter,
				sizeof(memCounter));
			if (result) {
				if (memCounter.PagefileUsage != mCurrentUsage) {
					mCurrentUsage = memCounter.PagefileUsage;
					emit memoryUsageChanged(mCurrentUsage);
				}
			}
#endif
		}

		void StatsModel::setProcess(HANDLE handle)
		{
			mHandle = handle;
			if (!mTimerId) {
				mTimerId = startTimer(1000);
			}
		}

		/*void StatsModel::trackAllocations()
		{
			startTrack({});
		}

		void StatsModel::logTrackedAllocations()
		{
			stopTrack({});
		}*/

		void StatsModel::dummy()
		{
		}

	}
}
