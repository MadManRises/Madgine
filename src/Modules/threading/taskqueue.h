#pragma once

#include "task.h"

#include "Generic/lambda.h"

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
        virtual ~TaskQueue();

        void queueHandle(TaskHandle &&task, const std::vector<Threading::DataMutex *> &dependencies = {});
        void queue_after(TaskHandle &&task, std::chrono::steady_clock::duration duration, const std::vector<Threading::DataMutex *> &dependencies = {});
        void queue_for(TaskHandle &&task, std::chrono::steady_clock::time_point time_point, const std::vector<Threading::DataMutex *> &dependencies = {});

        template <typename T, typename I>
        void queueTask(Task<T, I> task)
        {
            auto handle = std::move(task).assign(this);
            if (handle)
                queueHandle(std::move(handle));
        }

        template <typename F>
        void queue(F &&f)
        {
            queueTask(make_task(std::forward<F>(f)));
        }

        void addRepeatedTask(std::function<void()> &&task, std::chrono::steady_clock::duration interval = std::chrono::steady_clock::duration::zero(), void *owner = nullptr);
        void removeRepeatedTasks(void *owner);

        virtual std::optional<TaskTracker> fetch(std::chrono::steady_clock::time_point &nextTask, int &idleCount, int &repeatedCount);

        virtual bool idle() const;

        const std::string &name() const;

        std::chrono::steady_clock::time_point update(int idleCount = -1, int repeatedCount = -1);
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
            auto initHandle = std::move(initTask).assign(this);
            auto finalizeHandle = make_task(LIFT(TupleUnpacker::invoke), std::forward<Finalize>(finalize), std::move(future)).assign(this);            
            addSetupStepTasks(std::move(initHandle), std::move(finalizeHandle));
        }

        template <typename Init>
        void addSetupSteps(Init &&init)
        {
            auto initHandle = make_task(std::forward<Init>(init)).assign(this);
            addSetupStepTasks(std::move(initHandle));
        }

        bool await_ready();
        void await_suspend(TaskHandle handle);
        void await_resume();

    protected:
        struct ScheduledTask {
            TaskHandle mTask;
            std::chrono::steady_clock::time_point mScheduledFor = std::chrono::steady_clock::time_point::min();
        };

        void queueInternal(ScheduledTask &&tracker);

        virtual std::optional<TaskTracker> fetch_on_idle();

        TaskTracker wrapTask(TaskHandle &&task);

        void addSetupStepTasks(TaskHandle init, TaskHandle finalize = {});

    private:
        std::string mName;

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
        std::list<std::pair<TaskHandle, TaskHandle>>::iterator mSetupState;

        mutable std::mutex mMutex;
        std::condition_variable mCv;

        bool mWantsMainThread;
    };

}
}