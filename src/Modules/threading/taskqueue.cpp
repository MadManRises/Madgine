#include "../moduleslib.h"

#include "taskqueue.h"
#include "workgroup.h"

namespace Engine {
namespace Threading {

    bool TaskQualifiers::await_ready()
    {
        return false;
    }

    void TaskQualifiers::await_suspend(TaskHandle handle)
    {
        handle.queue()->queueHandle(std::move(handle), std::move(*this));
    }

    void TaskQualifiers::await_resume()
    {
    }

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
        assert(WorkGroup::self().state() != WorkGroupState::DONE);
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
        if (running()) {
            std::unique_lock<std::mutex> lock(mMutex);
            mCv.wait_until(lock, until);
        }
    }

    bool TaskQueue::running() const
    {
        return WorkGroup::self().state() == WorkGroupState::RUNNING;
    }

    void TaskQueue::stop()
    {
        WorkGroup::self().stop();
    }

    void TaskQueue::queueHandle(TaskHandle task, TaskQualifiers qualifiers)
    {
        queueInternal({ std::move(task), std::move(qualifiers) });
    }

    void TaskQueue::increaseTaskInFlightCount()
    {
        ++mTaskInFlightCount;
    }

    void TaskQueue::decreaseTaskInFlightCount()
    {
        --mTaskInFlightCount;
    }

    size_t TaskQueue::taskInFlightCount() const
    {
        return mTaskInFlightCount;
    }

    TaskHandle TaskQueue::fetch(std::chrono::steady_clock::time_point &nextTask)
    {
        std::chrono::steady_clock::time_point nextTaskTimepoint = nextTask;

        WorkGroupState state = WorkGroup::self().state();
        if (state == WorkGroupState::INITIALIZING) {
            while (mSetupState != mSetupSteps.end()) {
                TaskHandle init = mSetupState->first.assign(this);
                ++mSetupState;
                if (init) {
                    return init;
                }
            }
        } else if (state != WorkGroupState::DONE) {
            {
                std::lock_guard<std::mutex> lock(mMutex);
                auto now = std::chrono::steady_clock::now();
                for (auto it = mQueue.begin(); it != mQueue.end(); ++it) {
                    if ((it->mQualifiers.mScheduledFor <= now || WorkGroup::self().state() == WorkGroupState::STOPPING) && (!it->mQualifiers.mMutex || it->mQualifiers.mMutex->tryLock(this))) {
                        TaskHandle task = std::move(it->mTask);
                        mQueue.erase(it);
                        return task;
                    } else {
                        nextTaskTimepoint = std::min(it->mQualifiers.mScheduledFor.revert(), nextTaskTimepoint);
                    }
                }
            }

            if (state == WorkGroupState::FINALIZING) {
                while (mSetupState != mSetupSteps.begin()) {
                    --mSetupState;
                    TaskHandle finalize = mSetupState->second.assign(this);
                    if (finalize) {
                        return finalize;
                    }
                }
            }
        }

        nextTask = nextTaskTimepoint;
        return {};
    }

    bool TaskQueue::idle() const
    {
        switch (WorkGroup::self().state()) {
        case WorkGroupState::INITIALIZING:
            return mSetupState == mSetupSteps.end();
        case WorkGroupState::FINALIZING:
            return mSetupState == mSetupSteps.begin() && mTaskInFlightCount == 0;
        default:
            return mTaskInFlightCount == 0;
        }
    }

    void TaskQueue::notify()
    {
        mCv.notify_all();
    }

    void TaskQueue::addSetupStepTasks(Task<bool> init, Task<void> finalize)
    {
        assert(WorkGroup::self().state() == WorkGroupState::INITIALIZING);
        bool isItEnd = mSetupState == mSetupSteps.end();
        mSetupSteps.emplace_back(std::move(init), std::move(finalize));
        if (isItEnd) {
            mSetupState = std::prev(mSetupState);
        }
    }

}
}