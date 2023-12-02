#pragma once

#include "task.h"

#include "customclock.h"

#if ENABLE_TASK_TRACKING
#    include "../debug/tasktracking/tasktracker.h"
#endif

namespace Engine {
namespace Threading {

    struct MODULES_EXPORT TaskQualifiers {

        TaskQualifiers() = default;

        TaskQualifiers(CustomTimepoint timepoint)
            : mScheduledFor(timepoint)
        {
        }

        bool await_ready();
        void await_suspend(TaskHandle handle);
        void await_resume();

        CustomTimepoint mScheduledFor = std::chrono::steady_clock::now();
    };

    struct MODULES_EXPORT TaskQueue {

        TaskQueue(const std::string &name, bool wantsMainThread = false);
        ~TaskQueue();

        void queueHandle(TaskHandle task, TaskQualifiers qualifiers = {});

        template <typename T, bool Immediate>
        TaskFuture<T> queueTask(Task<T, Immediate> task, TaskQualifiers qualifiers = {})
        {
            TaskFuture<T> fut = task.get_future();
            TaskHandle handle = task.assign(this);
            if (handle)
                queueHandle(std::move(handle), std::move(qualifiers));
            return fut;
        }

        template <typename F>
        void queue(F &&f, TaskQualifiers qualifiers = {})
        {
            TaskHandle handle = make_task(std::forward<F>(f)).assign(this);
            if (handle)
                queueHandle(std::move(handle), std::move(qualifiers));
        }

        void increaseTaskInFlightCount();
        void decreaseTaskInFlightCount();
        size_t taskInFlightCount() const;

        TaskHandle fetch();

        bool idle() const;
        void notify();

        const std::string &name() const;

        void update(int taskCount = -1);

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

#if ENABLE_TASK_TRACKING
        Debug::Threading::TaskTracker mTracker;
#endif

    protected:
        struct ScheduledTask {
            TaskHandle mTask;
            TaskQualifiers mQualifiers;
        };

        void queueInternal(ScheduledTask tracker);

        void addSetupStepTasks(Task<bool> init, Task<void> finalize = {});

    private:
        std::string mName;
        bool mWantsMainThread;

        WorkGroup &mWorkGroup;

        std::atomic<size_t> mTaskInFlightCount = 0;

        std::list<ScheduledTask> mQueue;
        std::list<std::pair<Task<bool>, Task<void>>> mSetupSteps;
        std::list<std::pair<Task<bool>, Task<void>>>::iterator mSetupState = mSetupSteps.begin();

        mutable std::mutex mMutex;
        std::condition_variable mCv;
    };

}
}