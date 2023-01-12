#pragma once

#include "task.h"

#if ENABLE_TASK_TRACKING
#include "../debug/tasktracking/tasktracker.h"
#endif

namespace Engine {
namespace Threading {

    struct MODULES_EXPORT TaskQueue {
        TaskQueue(const std::string &name, bool wantsMainThread = false);
        ~TaskQueue();

        void queueHandle(TaskHandle task);
        void queueHandle_after(TaskHandle task, std::chrono::steady_clock::duration duration);
        void queueHandle_for(TaskHandle task, std::chrono::steady_clock::time_point time_point);

        template <typename T>
        TaskFuture<T> queueTask(Task<T> task)
        {
            TaskFuture<T> fut = task.get_future();
            TaskHandle handle = task.assign(this);
            if (handle)
                queueHandle(std::move(handle));
            return fut;
        }
        template <typename T>
        TaskFuture<T> queueTask_after(Task<T> task, std::chrono::steady_clock::duration duration)
        {
            TaskFuture<T> fut = task.get_future();
            TaskHandle handle = task.assign(this);
            if (handle)
                queueHandle_after(std::move(handle), duration);
            return fut;
        }
        template <typename T>
        TaskFuture<T> queueTask_for(Task<T> task, std::chrono::steady_clock::time_point time_point)
        {
            TaskFuture<T> fut = task.get_future();
            TaskHandle handle = task.assign(this);
            if (handle)
                queueHandle_for(std::move(handle), time_point);
            return fut;
        }

        template <typename F>
        void queue(F &&f)
        {
            TaskHandle handle = make_task(std::forward<F>(f)).assign(this);
            if (handle)
                queueHandle(std::move(handle));
        }
        template <typename F>
        void queue_after(F &&f, std::chrono::steady_clock::duration duration)
        {
            TaskHandle handle = make_task(std::forward<F>(f)).assign(this);
            if (handle)
                queueHandle_after(std::move(handle), duration);
        }
        template <typename F>
        void queue_for(F &&f, std::chrono::steady_clock::time_point time_point)
        {
            TaskHandle handle = make_task(std::forward<F>(f)).assign(this);
            if (handle)
                queueHandle_for(std::move(handle), time_point);
        }

        void increaseTaskInFlightCount();
        void decreaseTaskInFlightCount();
        size_t tasksInFlightCount() const;

        TaskHandle fetch(std::chrono::steady_clock::time_point &nextTask);

        bool idle() const;

        const std::string &name() const;

        std::chrono::steady_clock::time_point update(int taskCount = -1);
        void waitForTasks(std::chrono::steady_clock::time_point until = std::chrono::steady_clock::time_point::max());

        bool running() const;
        void stop();

        bool wantsMainThread() const;

        template <typename Init, typename Finalize>
        void addSetupSteps(Init &&init, Finalize &&finalize)
        {
            auto initTask = make_task(std::forward<Init>(init));
            auto future = initTask.get_future();
            auto finalizeTask = make_task(LIFT(TupleUnpacker::invoke), std::forward<Finalize>(finalize), std::move(future));
            addSetupStepTasks(std::move(initTask), std::move(finalizeTask));
        }

        template <typename Init>
        void addSetupSteps(Init &&init)
        {
            auto initTask = make_task(std::forward<Init>(init));
            addSetupStepTasks(std::move(initTask));
        }

        bool await_ready();
        bool await_suspend(TaskHandle handle);
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

        void addSetupStepTasks(Task<bool> init, Task<void> finalize = {});

    private:
        std::string mName;
        bool mWantsMainThread;

        std::atomic<bool> mRunning = true;
        std::atomic<size_t> mTaskInFlightCount = 0;

        std::list<ScheduledTask> mQueue;
        std::stack<TaskHandle> mAwaiterStack;
        std::list<std::pair<Task<bool>, Task<void>>> mSetupSteps;
        std::list<std::pair<Task<bool>, Task<void>>>::iterator mSetupState = mSetupSteps.begin();

        mutable std::mutex mMutex;
        std::condition_variable mCv;
    };

}
}