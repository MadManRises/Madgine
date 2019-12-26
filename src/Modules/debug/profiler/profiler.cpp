#include "../../moduleslib.h"

#include "profiler.h"

#include "../../threading/workgroupstorage.h"

#include "../../threading/threadlocal.h"

#include "../../signalslot/taskguard.h"

namespace Engine {

namespace Debug {

    namespace Profiler {

        THREADLOCAL(ProfilerThread)
        sThread;
        Threading::WorkgroupLocal<Profiler> sProfiler;
#if ENABLE_THREADING
        SignalSlot::TaskGuard _reg { []() { Threading::WorkGroup::addStaticThreadInitializer([]() { sProfiler->registerThread(&sThread); sThread->updateName(); }); }, []() {} };
#endif

        Profiler::Profiler()
        {
#if !ENABLE_THREADING
            registerThread(&sThread);
#endif
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
            : mStats("<unnamed-thread>")
        {
            updateName();
            mCurrent = &mStats;
            mStats.start();
        }

        ProfilerThread::~ProfilerThread()
        {
            mStats.stop();

#if ENABLE_THREADING
            if (Threading::WorkGroup::isInitialized())
                sProfiler->unregisterThread(this);
#endif
        }

        void ProfilerThread::updateName()
        {
#if ENABLE_THREADING
            mId = Threading::getCurrentThreadName();
#else
            mId = "Main";
#endif
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
