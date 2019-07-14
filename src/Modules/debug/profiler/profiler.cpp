#include "../../moduleslib.h"

#include "profiler.h"

#include "../../threading/workgroup.h"

namespace Engine {

namespace Debug {

    namespace Profiler {

        THREADLOCAL(ProfilerThread) sThread;
        Threading::WorkgroupLocal<Profiler> sProfiler;
        SignalSlot::TaskGuard _reg { []() { Threading::WorkGroup::addStaticThreadInitializer([]() { sProfiler->registerThread(&sThread); sThread->updateName(); }); }, []() {} };

        Profiler::Profiler()
        {
        }

        Profiler::~Profiler()
        {
        }

        const std::list<ProfilerThread *> &Profiler::getThreadStats()
        {
            return mThreads;
        }

        void Profiler::registerThread(ProfilerThread *thread)
        {
            mThreads.emplace_back(thread);
        }

        void Profiler::unregisterThread(ProfilerThread *thread)
        {
            mThreads.remove(thread);
        }

        Profiler &Profiler::getCurrent()
        {
            return sProfiler;
        }

        ProfilerThread::ProfilerThread()
            : mId(Threading::getCurrentThreadName())
            , mStats(mId.c_str())
        {
            mCurrent = &mStats;
            mStats.start();
        }

        ProfilerThread::~ProfilerThread()
        {
            mStats.stop();

			if (Threading::WorkGroup::isInitialized())
				sProfiler->unregisterThread(this);
        }

		void ProfilerThread::updateName() 
		{
                    mId = Threading::getCurrentThreadName();
                    mStats.setFunctionName(mId.c_str());
		}

        void StaticProcess::start()
        {
            if (mStats.start()) {
                mPrevious = sThread->mCurrent;
                sThread->mCurrent = &mStats;
            }
        }

        void StaticProcess::stop()
        {
            std::optional<ProcessStats::Data> d = mStats.stop();
            if (d) {
                assert(sThread->mCurrent == &mStats);
                sThread->mCurrent = mPrevious;
                if (mPrevious) {
                    mPrevious = nullptr;
                    if (!mParent) {
                        mParent = sThread->mCurrent->updateChild(&mStats, *d);
                    } else {
                        mParent->second += *d;
                    }
                }
            }
        }

    }
}
}
