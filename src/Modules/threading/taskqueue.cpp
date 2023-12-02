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
        , mWorkGroup(WorkGroup::self())
    {
        mWorkGroup.addTaskQueue(this);
    }

    TaskQueue::~TaskQueue()
    {
        assert(mQueue.empty());
        assert(mTaskInFlightCount == 0);
        mWorkGroup.removeTaskQueue(this);
    }

    bool TaskQueue::wantsMainThread() const
    {
        return mWantsMainThread;
    }

    void TaskQueue::queueInternal(ScheduledTask task)
    {
        assert(mWorkGroup.state() != WorkGroupState::DONE);
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

    void TaskQueue::update(int taskCount)
    {
        while (TaskHandle task = fetch()) {
            task();
            if (taskCount > 0) {
                --taskCount;
                if (taskCount == 0) {
                    return;
                }
            }
        }
    }

    bool TaskQueue::running() const
    {
        return mWorkGroup.state() == WorkGroupState::RUNNING;
    }

    void TaskQueue::stop()
    {
        mWorkGroup.stop();
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

    TaskHandle TaskQueue::fetch()
    {
        WorkGroupState state = mWorkGroup.state();
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
                std::unique_lock<std::mutex> lock(mMutex);
                while (!mQueue.empty()) {
                    auto now = std::chrono::steady_clock::now();
                    auto nextTaskTimepoint = std::chrono::steady_clock::time_point::max();
                    for (auto it = mQueue.begin(); it != mQueue.end(); ++it) {
                        if (it->mQualifiers.mScheduledFor <= now || mWorkGroup.state() == WorkGroupState::STOPPING) {
                            TaskHandle task = std::move(it->mTask);
                            mQueue.erase(it);
                            return task;
                        } else {
                            nextTaskTimepoint = std::min(it->mQualifiers.mScheduledFor.revert(), nextTaskTimepoint);
                        }
                    }
                    if (nextTaskTimepoint <= now) {
                        lock.unlock();
                        lock.lock();
                    } else
                        mCv.wait_until(lock, nextTaskTimepoint);                                        
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

        return {};
    }

    bool TaskQueue::idle() const
    {
        switch (mWorkGroup.state()) {
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
        assert(mWorkGroup.state() == WorkGroupState::INITIALIZING);
        bool isItEnd = mSetupState == mSetupSteps.end();
        mSetupSteps.emplace_back(std::move(init), std::move(finalize));
        if (isItEnd) {
            mSetupState = std::prev(mSetupState);
        }
    }

}
}