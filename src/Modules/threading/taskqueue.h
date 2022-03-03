#pragma once

#include "task.h"

#if ENABLE_TASK_TRACKING
#include "../debug/tasktracking/tasktracker.h"
#endif

namespace Engine {
namespace Threading {

    struct MODULES_EXPORT TaskTracker {
        TaskTracker(TaskHandle &&task, std::atomic<size_t> &tracker);
        TaskTracker(const TaskTracker &) = delete;
        TaskTracker(TaskTracker &&other);
        ~TaskTracker();

        TaskHandle mTask;

    private:
        std::atomic<size_t> *mTracker;
    };

    struct MODULES_EXPORT TaskQueue {
        TaskQueue(const std::string &name, bool wantsMainThread = false);
        ~TaskQueue();

        void queueHandle(TaskHandle task, const std::vector<Threading::DataMutex *> &dependencies = {});
        void queueHandle_after(TaskHandle task, std::chrono::steady_clock::duration duration, const std::vector<Threading::DataMutex *> &dependencies = {});
        void queueHandle_for(TaskHandle task, std::chrono::steady_clock::time_point time_point, const std::vector<Threading::DataMutex *> &dependencies = {});

        template <typename T, typename I>
        void queueTask(Task<T, I> task)
        {
            auto handle = std::move(task).assign(this);
            if (handle)
                queueHandle(std::move(handle));
        }
        template <typename T, typename I>
        void queueTask_after(Task<T, I> task, std::chrono::steady_clock::duration duration)
        {
            auto handle = std::move(task).assign(this);
            if (handle)
                queueHandle_after(std::move(handle), duration);
        }
        template <typename T, typename I>
        void queueTask_for(Task<T, I> task, std::chrono::steady_clock::time_point time_point)
        {
            auto handle = std::move(task).assign(this);
            if (handle)
                queueHandle_for(std::move(handle), time_point);
        }

        template <typename F>
        void queue(F &&f)
        {
            queueTask(make_task(std::forward<F>(f)));
        }
        template <typename F>
        void queue_after(F &&f, std::chrono::steady_clock::duration duration)
        {
            queueTask_after(make_task(std::forward<F>(f)), duration);
        }
        template <typename F>
        void queue_for(F &&f, std::chrono::steady_clock::time_point time_point)
        {
            queueTask_for(make_task(std::forward<F>(f)), time_point);
        }

        void addRepeatedTask(std::function<void()> task, std::chrono::steady_clock::duration interval = std::chrono::steady_clock::duration::zero(), void *owner = nullptr);
        void removeRepeatedTasks(void *owner);

        std::optional<TaskTracker> fetch(std::chrono::steady_clock::time_point &nextTask, int &repeatedCount);

        bool idle() const;

        const std::string &name() const;

        std::chrono::steady_clock::time_point update(int repeatedCount = -1);
        void waitForTasks(std::chrono::steady_clock::time_point until = std::chrono::steady_clock::time_point::max());

        void notify();

        bool running() const;
        void stop();

        bool wantsMainThread() const;

        template <typename Init, typename Finalize>
        void addSetupSteps(Init &&init, Finalize &&finalize)
        {
            auto initTask = make_task(std::forward<Init>(init));
            auto future = initTask.get_future();
            auto finalizeTask = make_task(LIFT(TupleUnpacker::invoke), std::forward<Finalize>(finalize), std::move(future));
            addSetupStepTasks(initTask.assign(this), finalizeTask.assign(this));
        }

        template <typename Init>
        void addSetupSteps(Init &&init)
        {
            auto initTask = make_task(std::forward<Init>(init));
            addSetupStepTasks(initTask.assign(this));
        }

        bool await_ready();
        void await_suspend(TaskHandle handle);
        void await_resume();

#if ENABLE_TASK_TRACKING
        Debug::Threading::TaskTracker mTracker;
#endif

    protected:
        struct ScheduledTask {
            TaskHandle mTask;
            std::chrono::steady_clock::time_point mScheduledFor = std::chrono::steady_clock::time_point::min();
        };

        void queueInternal(ScheduledTask tracker);

        TaskTracker wrapTask(TaskHandle task);

        void addSetupStepTasks(TaskHandle init, TaskHandle finalize = {});

    private:
        std::string mName;
        bool mWantsMainThread;

        std::atomic<bool> mRunning = true;
        std::atomic<size_t> mTaskCount = 0;

        struct RepeatedTask {
            std::function<void()> mTask;
            void *mOwner = nullptr;
            std::chrono::steady_clock::duration mInterval = std::chrono::steady_clock::duration::zero();
            std::chrono::steady_clock::time_point mNextExecuted = std::chrono::steady_clock::time_point::min();
        };

        std::list<ScheduledTask> mQueue;
        std::stack<TaskHandle> mAwaiterStack;
        std::vector<RepeatedTask> mRepeatedTasks;
        std::list<std::pair<TaskHandle, TaskHandle>> mSetupSteps;
        std::list<std::pair<TaskHandle, TaskHandle>>::iterator mSetupState = mSetupSteps.begin();

        mutable std::mutex mMutex;
        std::condition_variable mCv;
    };

}
}