#pragma once

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

    }
}
}

#if ENABLE_PROFILER
#    include "../../threading/threadlocal.h"
#    include "staticprocess.h"

#    define PROFILE_NAMED(name)                                                      \
        static THREADLOCAL(Engine::Debug::Profiler::StaticProcess) __sProcess(name); \
        Engine::Debug::Profiler::ProfileGuard<Engine::Debug::Profiler::StaticProcess> __profileGuard { __sProcess }
#else
#    define PROFILE_NAMED(name)
#endif
#define PROFILE() PROFILE_NAMED(__FUNCTION__)