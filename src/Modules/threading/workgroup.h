#pragma once

#include "../generic/future.h"
#include "task.h"

#if ENABLE_THREADING
#    include "Interfaces/threading/threadapi.h"
#endif

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
            createNamedThread("thread_" + std::to_string(++mInstanceCounter), std::forward<F>(main), std::forward<Args>(args)...);
        }

        template <typename F, typename... Args>
        void createNamedThread(std::string name, F &&main, Args &&... args)
        {
            mSubThreads.emplace_back(
                std::async(std::launch::async, &WorkGroup::threadMain<F, std::decay_t<Args>...>, this, std::move(name), std::forward<F>(main), std::forward<Args>(args)...));
        }

        template <typename F, typename... Args>
        auto spawnTaskThread(F &&task, Args &&... args)
        {
            return spawnNamedTaskThread("task_" + std::to_string(++mInstanceCounter), std::forward<F>(task), std::forward<Args>(args)...);
        }

        template <typename F, typename... Args>
        Future<std::invoke_result_t<F &&, Args &&...>> spawnNamedTaskThread(std::string name, F &&task, Args &&... args)
        {
            return std::async(std::launch::async, &WorkGroup::taskMain<F, std::decay_t<Args>...>, this, std::move(name), std::forward<F>(task), std::forward<Args>(args)...);
        }

        bool singleThreaded();
        void checkThreadStates();
#endif

        void addThreadInitializer(Threading::TaskHandle &&task);
        static void addStaticThreadInitializer(Threading::TaskHandle &&task);

        const std::string &name() const;

        static WorkGroup &self();
        static bool isInitialized();

        void addTaskQueue(TaskQueue *taskQueue);
        const std::vector<TaskQueue *> taskQueues() const;

    private:
#if ENABLE_THREADING
        void initThread(const std::string &name);
        void finalizeThread();

        struct ThreadGuard {
            ThreadGuard(const std::string &name, WorkGroup &group)
                : mGroup(group)
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
            ThreadGuard guard(name, *this);
            try {
                return TupleUnpacker::invokeDefaultResult(0, std::forward<F>(main), std::forward<Args>(args)...);
            } catch (std::exception &e) {
                LOG_ERROR("Uncaught Exception in Workgroup-Thread!");
                LOG_EXCEPTION(e);
                throw;
            }
        }

        template <typename F, typename... Args>
        auto taskMain(const std::string &name, F &&main, Args &&... args)
        {
            ThreadGuard guard(name, *this);
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
        std::vector<TaskHandle> mThreadInitializers;

        std::vector<TaskQueue *> mTaskQueues;
    };
}
}
