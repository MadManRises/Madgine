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
            task.mTask.setQueue(this);
            mQueue.emplace_back(std::move(task));
        }
        mCv.notify_one();
    }

    std::optional<TaskTracker> TaskQueue::fetch_on_idle()
    {
        return {};
    }

    TaskTracker TaskQueue::wrapTask(TaskHandle &&task, TaskMask mask)
    {
        return TaskTracker { std::move(task), mask, mTaskCount };
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
            assert(!f->mTask);
        }
        return nextAvailableTaskTime;
    }

    void TaskQueue::waitForTasks(const std::atomic<bool> *interruptFlag, std::chrono::steady_clock::time_point until)
    {
        std::unique_lock<std::mutex> lock(mMutex);

        if (until == std::chrono::steady_clock::time_point::max()) {
            auto cond = [=, &interruptFlag]() { return (interruptFlag && *interruptFlag) || !mQueue.empty() || !mRunning || !mRepeatedTasks.empty(); };
            mCv.wait(lock, cond);
        } else {
            auto cond = [=, &interruptFlag]() { return (interruptFlag && *interruptFlag) || !mQueue.empty() || !mRunning; };
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

    void TaskQueue::queueHandle(TaskHandle &&task, TaskMask mask, const std::vector<Threading::DataMutex *> &dependencies)
    {
        queueInternal({ std::move(task), mask });
    }

    void TaskQueue::queue_after(TaskHandle &&task, TaskMask mask, std::chrono::steady_clock::duration duration, const std::vector<Threading::DataMutex *> &dependencies)
    {
        queue_for(std::move(task), mask, std::chrono::steady_clock::now() + duration);
    }

    void TaskQueue::queue_for(TaskHandle &&task, TaskMask mask, std::chrono::steady_clock::time_point time_point, const std::vector<Threading::DataMutex *> &dependencies)
    {
        queueInternal({ std::move(task), mask, time_point });
    }

    void TaskQueue::addRepeatedTask(std::function<void()> &&task, TaskMask mask, std::chrono::steady_clock::duration interval, void *owner)
    {
        {
            std::lock_guard<std::mutex> lock(mMutex);
            mRepeatedTasks.emplace_back(RepeatedTask { std::move(task), mask, owner, interval });
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

        if (mRunning && match(TaskMask::DEFAULT, taskMask)) {
            while (mSetupState != mSetupSteps.end()) {
                Lambda<void()> init = std::move(mSetupState->first);
                ++mSetupState;
                if (init) {
                    return wrapTask(make_task(std::move(init)), TaskMask::DEFAULT);
                }
            }
        }

        {
            std::lock_guard<std::mutex> lock(mMutex);
            for (auto it = mQueue.begin(); it != mQueue.end(); ++it) {
                if (!match(it->mMask, taskMask))
                    continue;
                if (it->mScheduledFor <= std::chrono::steady_clock::now()) {
                    TaskTracker f = wrapTask(std::move(it->mTask), it->mMask);
                    mQueue.erase(it);
                    return f;
                } else {
                    nextTaskTimepoint = std::min(it->mScheduledFor, nextTaskTimepoint);
                }
            }
            if (mRunning && repeatedCount != 0 && (!interruptFlag || !*interruptFlag)) {
                RepeatedTask *nextTask = nullptr;
                for (RepeatedTask &task : mRepeatedTasks) {
                    if (!match(task.mMask, taskMask))
                        continue;
                    if (task.mNextExecuted < nextTaskTimepoint) {
                        nextTask = &task;
                        nextTaskTimepoint = task.mNextExecuted;
                    }
                }
                if (nextTask && nextTaskTimepoint <= std::chrono::steady_clock::now()) {
                    if (repeatedCount > 0)
                        --repeatedCount;
                    nextTask->mNextExecuted = std::chrono::steady_clock::now() + nextTask->mInterval;
                    return wrapTask(make_task([=]() { nextTask->mTask(); }), nextTask->mMask);
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

        if (!mRunning && match(TaskMask::DEFAULT, taskMask) && nextTaskTimepoint == std::chrono::steady_clock::time_point::max()) {

            while (mSetupState != mSetupSteps.begin()) {
                --mSetupState;
                Lambda<void()> finalize = std::move(mSetupState->second);
                if (finalize) {
                    return wrapTask(make_task(std::move(finalize)), TaskMask::DEFAULT);
                }
            }
        }

        nextTask = nextTaskTimepoint;
        return {};
    }

    bool TaskQueue::idle(TaskMask taskMask) const
    {
        if (match(TaskMask::DEFAULT, taskMask)) {
            if (mRunning) {
                if (mSetupState != mSetupSteps.end())
                    return false;
            } else {
                if (mSetupState != mSetupSteps.begin())
                    return false;
            }
        }

        for (const ScheduledTask &task : mQueue)
            if (match(task.mMask, taskMask))
                return false;

        return mTaskCount == 0;
    }

    void TaskQueue::addSetupSteps(Lambda<bool()> &&init, Lambda<void()> &&finalize)
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

    TaskTracker::TaskTracker(TaskHandle &&task, TaskMask mask, std::atomic<size_t> &tracker)
        : mTask(std::move(task))
        , mMask(mask)
        , mTracker(&tracker)
    {
        assert(mTask);
        ++tracker;
    }

    TaskTracker::TaskTracker(TaskTracker &&other)
        : mTask(std::move(other.mTask))
        , mMask(other.mMask)
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