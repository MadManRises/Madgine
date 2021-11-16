#pragma once

#include "Generic/future.h"



namespace Engine {
namespace Threading {

    struct MODULES_EXPORT WorkGroup {
        WorkGroup(const std::string &name = "");
        WorkGroup(const WorkGroup &) = delete;
        ~WorkGroup();

        void operator=(const WorkGroup &) = delete;

#if ENABLE_THREADING
        template <typename F, typename... Args>
        void createThread(F &&main, Args &&... args)
        {
            mSubThreads.emplace_back(
                std::async(std::launch::async, &WorkGroup::threadMain<F, std::decay_t<Args>...>, this, std::forward<F>(main), std::forward<Args>(args)...));
        }

        template <typename F, typename... Args>
        auto spawnTaskThread(F &&task, Args &&... args)
        {
            return std::async(std::launch::async, &WorkGroup::taskMain<F, std::decay_t<Args>...>, this, std::forward<F>(task), std::forward<Args>(args)...);
        }

        bool singleThreaded();
        void checkThreadStates();
#endif

        void addThreadInitializer(std::function<void()> &&task);
        static void addStaticThreadInitializer(std::function<void()> &&task);

        const std::string &name() const;

        static WorkGroup &self();
        static bool isInitialized();

        void addTaskQueue(TaskQueue *taskQueue);
        void removeTaskQueue(TaskQueue *taskQueue);
        const std::vector<TaskQueue *> taskQueues() const;

    private:
#if ENABLE_THREADING
        void initThread();
        void finalizeThread();

        struct ThreadGuard {
            ThreadGuard(WorkGroup &group)
                : mGroup(group)
            {
                mGroup.initThread();
            }

            ~ThreadGuard()
            {
                mGroup.finalizeThread();
            }

        private:
            WorkGroup &mGroup;
        };

        template <typename F, typename... Args>
        int threadMain(F &&main, Args &&... args)
        {
            ThreadGuard guard(*this);
            try {
                int result = TupleUnpacker::invokeDefaultResult(0, std::forward<F>(main), std::forward<Args>(args)...);
                return result;
            } catch (std::exception &e) {
                LOG_ERROR("Uncaught Exception in Workgroup-Thread!");
                LOG_EXCEPTION(e);
                throw;
            }
        }

        template <typename F, typename... Args>
        auto taskMain(F &&main, Args &&... args)
        {
            ThreadGuard guard(*this);
            try {
                return std::forward<F>(main)(std::forward<Args>(args)...);
            } catch (std::exception &e) {
                LOG_ERROR("Uncaught Exception in Workgroup-Task-Thread!");
                LOG_EXCEPTION(e);
                throw;
            }
        }
#endif

    private:
        friend struct WorkGroupStorage;

        size_t mInstanceCounter = 0;
        std::string mName;

#if ENABLE_THREADING
        std::vector<Future<int>> mSubThreads;
#endif
        std::vector<std::function<void()>> mThreadInitializers;

        std::vector<TaskQueue *> mTaskQueues;
    };
}
}
