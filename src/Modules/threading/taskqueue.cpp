#include "../moduleslib.h"

#include "taskqueue.h"
#include "workgroup.h"

namespace Engine {
namespace Threading {

    TaskQueue::TaskQueue(const std::string &name, bool wantsMainThread)
        : mName(name)
        , mWantsMainThread(wantsMainThread)
    {
        WorkGroup::self().addTaskQueue(this);
    }

    TaskQueue::~TaskQueue()
    {
        assert(mQueue.empty());
        assert(mTaskInFlightCount == 0);
        WorkGroup::self().removeTaskQueue(this);
    }

    bool TaskQueue::wantsMainThread() const
    {
        return mWantsMainThread;
    }

    void TaskQueue::queueInternal(ScheduledTask task)
    {
        {
            //TODO: priority Queue
            std::lock_guard<std::mutex> lock(mMutex);
            mQueue.emplace_back(std::move(task));
        }
        mCv.notify_one();
    }

    const std::string &TaskQueue::name() const
    {
        return mName;
    }

    std::chrono::steady_clock::time_point TaskQueue::update(int taskCount)
    {
        std::chrono::steady_clock::time_point nextAvailableTaskTime = std::chrono::steady_clock::time_point::max();
        while (TaskHandle task = fetch(nextAvailableTaskTime)) {
            task();
            if (taskCount > 0) {
                --taskCount;
                if (taskCount == 0) {
                    return std::chrono::steady_clock::time_point::min();
                }
            }
        }
        return nextAvailableTaskTime;
    }

    void TaskQueue::waitForTasks(std::chrono::steady_clock::time_point until)
    {
        std::unique_lock<std::mutex> lock(mMutex);
        mCv.wait_until(lock, until, [this]() { return !mRunning && idle(); });
    }

    bool TaskQueue::running() const
    {
        return mRunning;
    }

    void TaskQueue::stop()
    {
        {
            std::unique_lock<std::mutex> lock(mMutex);
            mRunning = false;
        }
        mCv.notify_all();
    }

    void TaskQueue::queueHandle(TaskHandle task)
    {
        queueInternal({ std::move(task) });
    }

    void TaskQueue::queueHandle_after(TaskHandle task, std::chrono::steady_clock::duration duration)
    {
        queueHandle_for(std::move(task), std::chrono::steady_clock::now() + duration);
    }

    void TaskQueue::queueHandle_for(TaskHandle task, std::chrono::steady_clock::time_point time_point)
    {
        queueInternal({ std::move(task), time_point });
    }

    void TaskQueue::increaseTaskInFlightCount()
    {
        ++mTaskInFlightCount;
    }

    void TaskQueue::decreaseTaskInFlightCount()
    {
        --mTaskInFlightCount;
    }

    size_t TaskQueue::tasksInFlightCount() const
    {
        return mTaskInFlightCount;
    }

    TaskHandle TaskQueue::fetch(std::chrono::steady_clock::time_point &nextTask)
    {
        std::chrono::steady_clock::time_point nextTaskTimepoint = nextTask;

        if (mRunning) {
            while (mSetupState != mSetupSteps.end()) {
                TaskHandle init = mSetupState->first.assign(this);
                ++mSetupState;
                if (init) {
                    return init;
                }
            }
        }

        {
            std::lock_guard<std::mutex> lock(mMutex);
            for (auto it = mQueue.begin(); it != mQueue.end(); ++it) {
                if (it->mScheduledFor <= std::chrono::steady_clock::now()) {
                    TaskHandle task = std::move(it->mTask);
                    mQueue.erase(it);
                    return task;
                } else {
                    nextTaskTimepoint = std::min(it->mScheduledFor, nextTaskTimepoint);
                }
            }
            if (mTaskInFlightCount == 0) {
                while (!mAwaiterStack.empty()) {
                    TaskHandle handle = std::move(mAwaiterStack.top());
                    mAwaiterStack.pop();
                    if (handle.queue() == this) {
                        return handle;
                    }
                    handle.resumeInQueue();
                }
            }
        }

        if (!mRunning && mTaskInFlightCount == 0) {

            while (mSetupState != mSetupSteps.begin()) {
                --mSetupState;
                TaskHandle finalize = mSetupState->second.assign(this);
                if (finalize) {
                    return finalize;
                }
            }
        }

        nextTask = nextTaskTimepoint;
        return {};
    }

    bool TaskQueue::idle() const
    {
        if (mTaskInFlightCount > 0)
            return false;

        if (mRunning) {
            return mSetupState == mSetupSteps.end();
        } else {
            return mSetupState == mSetupSteps.begin();
        }
    }

    void TaskQueue::addSetupStepTasks(Task<bool> init, Task<void> finalize)
    {
        bool isItEnd = mSetupState == mSetupSteps.end();
        mSetupSteps.emplace_back(std::move(init), std::move(finalize));
        if (isItEnd) {
            mSetupState = std::prev(mSetupState);
        }
    }

    bool TaskQueue::await_ready()
    {
        return idle();
    }

    bool TaskQueue::await_suspend(TaskHandle handle)
    {
        std::lock_guard<std::mutex> lock { mMutex };
        if (!idle()) {
            mAwaiterStack.emplace(std::move(handle));
            return true;
        } else {
            handle.release();
            return false;
        }
    }

    void TaskQueue::await_resume()
    {
    }

}
}