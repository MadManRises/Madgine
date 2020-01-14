#pragma once

namespace Engine {
namespace Debug {
    namespace Profiler {

        struct MODULES_EXPORT Profiler {
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

    }
}
}
