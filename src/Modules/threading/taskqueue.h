#pragma once

#include "task.h"

#include "Generic/lambda.h"

namespace Engine {
namespace Threading {

    enum class TaskMask : uint8_t {
        NONE = 0,
        DEFAULT = 1 << 0,
        BARRIER = 1 << 1,
        ALL = (1 << 8) - 1
    };

    MODULES_EXPORT bool match(TaskMask v, TaskMask values, TaskMask mask = TaskMask::NONE);

    struct MODULES_EXPORT TaskTracker {
        TaskTracker(TaskHandle &&task, TaskMask mask, std::atomic<size_t> &tracker);
        TaskTracker(const TaskTracker &) = delete;
        TaskTracker(TaskTracker &&other);
        ~TaskTracker();

        TaskHandle mTask;
        TaskMask mMask;

    private:
        std::atomic<size_t> *mTracker;
    };

    struct MODULES_EXPORT TaskQueue {
        TaskQueue(const std::string &name, bool wantsMainThread = false);
        virtual ~TaskQueue();

        void queueHandle(TaskHandle &&task, TaskMask mask, const std::vector<Threading::DataMutex *> &dependencies = {});
        void queue_after(TaskHandle &&task, TaskMask mask, std::chrono::steady_clock::duration duration, const std::vector<Threading::DataMutex *> &dependencies = {});
        void queue_for(TaskHandle &&task, TaskMask mask, std::chrono::steady_clock::time_point time_point, const std::vector<Threading::DataMutex *> &dependencies = {});

        template <typename T, typename I>
        TaskFuture<T> queueTask(Task<T, I> task, TaskMask mask)
        {
            auto [fut, handle] = std::move(task).release(this);
            if (handle)
                queueHandle(std::move(handle), mask);

            return std::move(fut);
        }

        template <typename F>
        auto queue(F &&f, TaskMask mask)
        {
            return queueTask(make_task(std::forward<F>(f)), mask);
        }

        void addRepeatedTask(std::function<void()> &&task, TaskMask mask, std::chrono::steady_clock::duration interval = std::chrono::steady_clock::duration::zero(), void *owner = nullptr);
        void removeRepeatedTasks(void *owner);

        virtual std::optional<TaskTracker> fetch(TaskMask taskMask, const std::atomic<bool> *interruptFlag, std::chrono::steady_clock::time_point &nextTask, int &idleCount, int &repeatedCount);

        virtual bool idle(TaskMask taskMask) const;

        const std::string &name() const;

        std::chrono::steady_clock::time_point update(TaskMask taskMask, const std::atomic<bool> *interruptFlag, int idleCount = -1, int repeatedCount = -1);
        void waitForTasks(const std::atomic<bool> *interruptFlag, std::chrono::steady_clock::time_point until = std::chrono::steady_clock::time_point::max());

        void notify();

        bool running() const;
        void stop();

        bool wantsMainThread() const;

        template <typename Init, typename Finalize>
        void addSetupSteps(Init &&init, Finalize &&finalize)
        {
            auto [future, initHandle] = make_task(std::forward<Init>(init)).release(this);
            auto [_, finalizeHandle] = make_task(LIFT(TupleUnpacker::invoke), std::forward<Finalize>(finalize), std::move(future)).release(this);            
            addSetupStepTasks(std::move(initHandle), std::move(finalizeHandle));
        }

        template <typename Init>
        void addSetupSteps(Init &&init)
        {
            auto [_, initHandle] = make_task(std::forward<Init>(init));
            addSetupStepTasks(std::move(initHandle));
        }

        bool await_ready();
        void await_suspend(TaskHandle handle);
        void await_resume();

    protected:
        struct ScheduledTask {
            TaskHandle mTask;
            TaskMask mMask;
            std::chrono::steady_clock::time_point mScheduledFor = std::chrono::steady_clock::time_point::min();
        };

        void queueInternal(ScheduledTask &&tracker);

        virtual std::optional<TaskTracker> fetch_on_idle();

        TaskTracker wrapTask(TaskHandle &&task, TaskMask mask);

        void addSetupStepTasks(TaskHandle init, TaskHandle finalize = {});

    private:
        std::string mName;

        std::atomic<bool> mRunning = true;
        std::atomic<size_t> mTaskCount = 0;

        struct RepeatedTask {
            std::function<void()> mTask;
            TaskMask mMask;
            void *mOwner = nullptr;
            std::chrono::steady_clock::duration mInterval = std::chrono::steady_clock::duration::zero();
            std::chrono::steady_clock::time_point mNextExecuted = std::chrono::steady_clock::time_point::min();
        };

        std::list<ScheduledTask> mQueue;
        std::stack<TaskHandle> mAwaiterStack;
        std::vector<RepeatedTask> mRepeatedTasks;
        std::list<std::pair<TaskHandle, TaskHandle>> mSetupSteps;
        std::list<std::pair<TaskHandle, TaskHandle>>::iterator mSetupState;

        mutable std::mutex mMutex;
        std::condition_variable mCv;

        bool mWantsMainThread;
    };

}
}