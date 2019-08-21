#pragma once

#include "threadlocal.h"

#if ENABLE_THREADING

#include "../generic/tupleunpacker.h"
#include "../signalslot/task.h"
#include "Interfaces/threading/threadapi.h"

namespace Engine {
namespace Threading {

    struct MODULES_EXPORT WorkGroup {
        WorkGroup(const std::string &name = "");
        WorkGroup(const WorkGroup &) = delete;
        ~WorkGroup();

		void operator=(const WorkGroup &) = delete;

        template <typename F, typename... Args>
        void createThread(F &&main, Args &&... args)
        {
            createNamedThread("thread_" + std::to_string(++mInstanceCounter), std::forward<F>(main), std::forward<Args>(args)...);
        }

        template <typename F, typename... Args>
        void createNamedThread(const std::string &name, F &&main, Args &&... args)
        {
            mSubThreads.emplace_back(
                std::async(std::launch::async, &WorkGroup::threadMain<F, std::decay_t<Args>...>, this, name, std::forward<F>(main), std::forward<Args>(args)...));
        }

        void addThreadInitializer(SignalSlot::TaskHandle &&task);
        static void addStaticThreadInitializer(SignalSlot::TaskHandle &&task);

        bool singleThreaded();
        void checkThreadStates();

        const std::string &name() const;

        static int registerLocalBssVariable(std::function<Any()> ctor);
        static void unregisterLocalBssVariable(int index);
        static int registerLocalObjectVariable(std::function<Any()> ctor);
        static void unregisterLocalObjectVariable(int index);

        static const Any &localVariable(int index);

        static WorkGroup &self();
        static bool isInitialized();

    private:
        void initThread(const std::string &name);
        void finalizeThread();

        struct ThreadGuard {
            ThreadGuard(const std::string &name, WorkGroup *group)
                : mGroup(*group)
            {
                mGroup.initThread(name);
            }

            ~ThreadGuard()
            {
                mGroup.finalizeThread();
            }

        private:
            WorkGroup &mGroup;
        };

        template <typename F, typename... Args>
        int threadMain(const std::string &name, F &&main, Args &&... args)
        {
            ThreadGuard guard(name, this);
            try {
                return TupleUnpacker::invokeDefaultResult(0, std::forward<F>(main), std::forward<Args>(args)..., *this);
            } catch (std::exception &e) {
                LOG_ERROR("Uncaught Exception in Workgroup-Thread!");
                LOG_EXCEPTION(e);
                throw;
            }
        }

    private:
        std::string mName;
        size_t mInstanceCounter = 0;

        std::vector<std::future<int>> mSubThreads;
        std::vector<SignalSlot::TaskHandle> mThreadInitializers;

        std::vector<Any> mBssVariables;
        std::vector<Any> mObjectVariables;
    };

    template <typename T>
    using WorkgroupLocal = ThreadLocal<T, WorkGroup>;

}
}

#else

namespace Engine {
namespace Threading {

    struct WorkGroup {
        WorkGroup(const std::string &name = "") {};
        ~WorkGroup() {};
    };

    template <typename T>
    using WorkgroupLocal = Proxy<T>;

}
}

#endif