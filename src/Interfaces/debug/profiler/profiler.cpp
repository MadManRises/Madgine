#include "../../interfaceslib.h"

#include "profiler.h"


namespace Engine
{

	namespace Debug {

		namespace Profiler
		{

			thread_local ProcessStats *sCurrent = nullptr;

			Profiler *Profiler::sSingleton = nullptr;

			Profiler::Profiler()
			{
				assert(!sSingleton);
				sSingleton = this;
			}

			Profiler::~Profiler()
			{
				assert(sSingleton == this);
				sSingleton = nullptr;
			}

			const ProcessStats * Profiler::getThreadStats()
			{
				return &mMainThread.mStats;
			}

			Profiler & Profiler::getSingleton()
			{
				assert(sSingleton);
				return *sSingleton;
			}

			ProfilerThread::ProfilerThread() :
				mStats("Thread - main()")
			{
				sCurrent = &mStats;
				mStats.start();
			}

			void StaticProcess::start()
			{
				if (mStats.start()) {
					mPrevious = sCurrent;
					sCurrent = &mStats;
				}
			}

			void StaticProcess::stop()
			{
				std::optional<ProcessStats::Data> d = mStats.stop();
				if (d) {
					assert(sCurrent == &mStats);
					sCurrent = mPrevious;
					mPrevious = nullptr;
					if (!mParent) {
						mParent = sCurrent->updateChild(&mStats, *d);
					}
					else {
						mParent->second += *d;
					}
				}
			}

		}
	}
}
