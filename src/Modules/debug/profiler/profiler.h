#pragma once

#include "processstats.h"

#include "../../threading/threadlocal.h"

namespace Engine {
namespace Debug {
    namespace Profiler {

        template <class T>
        struct ProfileGuard {
            ProfileGuard(T &t)
                : mT(t)
            {
                mT.start();
            }
            ~ProfileGuard()
            {
                mT.stop();
            }

        private:
            T &mT;
        };

        struct MODULES_EXPORT ProfilerThread {
            ProfilerThread();
            ProfilerThread(const ProfilerThread &) = delete;
            ProfilerThread(ProfilerThread &&) = delete;
            ~ProfilerThread();

			void updateName();

            std::string mId;

            ProcessStats mStats;
            ProcessStats *mCurrent;
        };

        class MODULES_EXPORT Profiler {
        public:
            Profiler();
            Profiler(const Profiler &) = delete;
            Profiler(Profiler &&) = delete;
            ~Profiler();

            Profiler &operator=(const Profiler &) = delete;

            const std::list<ProfilerThread *> &getThreadStats();

            void registerThread(ProfilerThread *thread);
            void unregisterThread(ProfilerThread *thread);

            static Profiler &getCurrent();

        private:
            std::list<ProfilerThread *> mThreads;
        };

        struct MODULES_EXPORT StaticProcess {
            StaticProcess(const char *function)
                : mStats(function)
            {
            }

            void start();
            void stop();

        private:
            ProcessStats mStats;

            ProcessStats *mPrevious = nullptr;

            std::pair<ProcessStats *const, ProcessStats::Data> *mParent = nullptr;
        };

#if ENABLE_PROFILER
#	define PROFILE_NAMED(name)                                                      \
		static THREADLOCAL(Engine::Debug::Profiler::StaticProcess) __sProcess(name); \
		Engine::Debug::Profiler::ProfileGuard<Engine::Debug::Profiler::StaticProcess> __profileGuard { __sProcess }
#else
#	define PROFILE_NAMED(name)
#endif
#define PROFILE() PROFILE_NAMED(__FUNCTION__)
    }
}
}
