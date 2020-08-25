#include "../moduleslib.h"

#include "taskqueue.h"
#include "workgroup.h"

namespace Engine {
namespace Threading {

    bool match(TaskMask v, TaskMask values, TaskMask mask)
    {
        if (mask == TaskMask::NONE)
            mask = values;
        return (static_cast<uint8_t>(v) & static_cast<uint8_t>(mask)) == static_cast<uint8_t>(values);
    }

    TaskQueue::TaskQueue(const std::string &name, bool wantsMainThread)
        : mName(name)
        , mSetupState(mSetupSteps.begin())
        , mWantsMainThread(wantsMainThread)
    {        
        WorkGroup::self().addTaskQueue(this);
    }

    TaskQueue::~TaskQueue()
    {
        WorkGroup::self().removeTaskQueue(this);
    }

    bool TaskQueue::wantsMainThread() const
    {
        return mWantsMainThread;
    }

    void TaskQueue::queueInternal(ScheduledTask &&task)
    {
        //TODO: priority Queue
        {
            std::lock_guard<std::mutex> lock(mMutex);
            mQueue.emplace_back(std::move(task));
        }
        mCv.notify_one();
    }

    std::optional<TaskTracker> TaskQueue::fetch_on_idle()
    {
        return {};
    }

    TaskTracker TaskQueue::wrapTask(TaskHandle &&task)
    {
        return TaskTracker { std::move(task), mTaskCount };
    }

    const std::string &TaskQueue::name() const
    {
        return mName;
    }

    std::chrono::steady_clock::time_point TaskQueue::update(TaskMask taskMask, const std::atomic<bool> *interruptFlag, int idleCount, int repeatedCount)
    {
        std::chrono::steady_clock::time_point nextAvailableTaskTime = std::chrono::steady_clock::time_point::max();
        while (std::optional<Threading::TaskTracker> f = fetch(taskMask, interruptFlag, nextAvailableTaskTime, idleCount, repeatedCount)) {
            f->mTask();
        }
        return nextAvailableTaskTime;
    }

    void TaskQueue::waitForTasks(const std::atomic<bool> *interruptFlag, std::chrono::steady_clock::time_point until)
    {
        std::unique_lock<std::mutex> lock(mMutex);        
        
        if (until == std::chrono::steady_clock::time_point::max()) {
            auto cond = [=, &interruptFlag]() { return interruptFlag || !mQueue.empty() || !mRunning || !mRepeatedTasks.empty(); };
            mCv.wait(lock, cond);
        } else {
            auto cond = [=, &interruptFlag]() { return interruptFlag || !mQueue.empty() || !mRunning; };
            mCv.wait_until(lock, until, cond);
        }
    }

    void TaskQueue::notify()
    {
        mCv.notify_one();
    }

    bool TaskQueue::running() const
    {
        return mRunning;
    }

    void TaskQueue::stop()
    {
        mRunning = false;
        mCv.notify_all();
    }

    void TaskQueue::queue(TaskHandle &&task, const std::vector<Threading::DataMutex *> &dependencies)
    {
        queueInternal({ wrapTask(std::move(task)) });
    }

    void TaskQueue::queue_after(TaskHandle &&task, std::chrono::steady_clock::duration duration, const std::vector<Threading::DataMutex *> &dependencies)
    {
        queue_for(std::move(task), std::chrono::steady_clock::now() + duration);
    }

    void TaskQueue::queue_for(TaskHandle &&task, std::chrono::steady_clock::time_point time_point, const std::vector<Threading::DataMutex *> &dependencies)
    {
        queueInternal({ wrapTask(std::move(task)), time_point });
    }

    void TaskQueue::addRepeatedTask(TaskHandle &&task, std::chrono::steady_clock::duration interval, void *owner)
    {
        {
            std::lock_guard<std::mutex> lock(mMutex);
            mRepeatedTasks.emplace_back(RepeatedTask { std::move(task), owner, interval });
        }
        mCv.notify_one();
    }

    void TaskQueue::removeRepeatedTasks(void *owner)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        mRepeatedTasks.erase(std::remove_if(mRepeatedTasks.begin(), mRepeatedTasks.end(), [owner](const RepeatedTask &task) { return task.mOwner == owner; }), mRepeatedTasks.end());        
    }

    std::optional<TaskTracker> TaskQueue::fetch(TaskMask taskMask, const std::atomic<bool> *interruptFlag, std::chrono::steady_clock::time_point &nextTask, int &idleCount, int &repeatedCount)
    {
        std::chrono::steady_clock::time_point nextTaskTimepoint = nextTask;

        if (mRunning && match(taskMask, TaskMask::DEFAULT)) {
            while (mSetupState != mSetupSteps.end()) {
                std::optional<Threading::TaskHandle> init = std::move(mSetupState->first);
                ++mSetupState;
                if (init) {
                    return wrapTask(std::move(*init));
                }
            }
        }

        {
            std::lock_guard<std::mutex> lock(mMutex);
            if (!mQueue.empty()) {
                ScheduledTask &task = mQueue.front();
                if (task.mScheduledFor <= std::chrono::steady_clock::now()) {
                    TaskTracker f = std::move(task.mTask);
                    mQueue.pop_front();
                    return f;
                } else {
                    nextTaskTimepoint = std::min(task.mScheduledFor, nextTaskTimepoint);
                }
            }
            if (mRunning && repeatedCount != 0 && (!interruptFlag || !*interruptFlag)) {
                RepeatedTask *nextTask = nullptr;
                for (RepeatedTask &task : mRepeatedTasks) {                    
                    if (task.mNextExecuted < nextTaskTimepoint) {
                        nextTask = &task;
                        nextTaskTimepoint = task.mNextExecuted;                    
                    }
                }
                if (nextTask && nextTaskTimepoint <= std::chrono::steady_clock::now()) {
                    if (repeatedCount > 0)
                        --repeatedCount;
                    nextTask->mNextExecuted = std::chrono::steady_clock::now() + nextTask->mInterval;
                    return wrapTask([=]() { nextTask->mTask(); });
                }
            }
        }

        if (mRunning && idleCount != 0 && (!interruptFlag || !*interruptFlag)) {
            size_t zero = 0;
            if (mTaskCount.compare_exchange_strong(zero, 1)) {
                std::optional<TaskTracker> task = fetch_on_idle();
                --mTaskCount;
                if (task) {
                    if (idleCount > 0)
                        --idleCount;
                    return task;
                }
            }
        }

        if (!mRunning && match(taskMask, TaskMask::DEFAULT) && nextTaskTimepoint == std::chrono::steady_clock::time_point::max()) {

            while (mSetupState != mSetupSteps.begin()) {
                --mSetupState;
                Threading::TaskHandle finalize = std::move(mSetupState->second);
                if (finalize) {
                    return wrapTask(std::move(finalize));
                }
            }
        }

        nextTask = nextTaskTimepoint;
        return {};
    }

    bool TaskQueue::idle(TaskMask taskMask) const
    {
        return mTaskCount == 0 && (!match(taskMask, TaskMask::DEFAULT) || mSetupState == mSetupSteps.end() || !mRunning);
    }

    void TaskQueue::addSetupSteps(std::function<bool()> &&init, TaskHandle &&finalize)
    {
        bool isItEnd = mSetupState == mSetupSteps.end();
        if (init && finalize) {
            std::promise<bool> p;
            std::future<bool> f = p.get_future();
            mSetupSteps.emplace_back(
                [init { std::move(init) }, p { std::move(p) }]() mutable {
                    try {
                        p.set_value(init());
                    } catch (std::exception &) {
                        p.set_value(false);
                        throw;
                    }
                },
                [finalize { std::move(finalize) }, f { std::move(f) }]() mutable {
                    if (f.get())
                        finalize();
                });
        } else {
            mSetupSteps.emplace_back(std::move(init), std::move(finalize));
        }
        if (isItEnd) {
            mSetupState = std::prev(mSetupState);
        }
    }

    TaskTracker::TaskTracker(TaskHandle &&task, std::atomic<size_t> &tracker)
        : mTask(std::move(task))
        , mTracker(&tracker)
    {
        assert(mTask);
        ++tracker;
    }

    TaskTracker::TaskTracker(TaskTracker &&other)
        : mTask(std::move(other.mTask))
        , mTracker(std::exchange(other.mTracker, nullptr))
    {
    }

    TaskTracker::~TaskTracker()
    {
        if (mTracker)
            --(*mTracker);
    }

}
}