#include "../moduleslib.h"

#include "taskqueue.h"

namespace Engine {
namespace Threading {

    TaskQueue::TaskQueue(const std::string &name)
        : mName(name)
    {
    }

    void TaskQueue::queueInternal(ScheduledTask &&task)
    {
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

    void TaskQueue::update(int idleCount)
    {
        std::chrono::steady_clock::time_point nextAvailableTaskTime;
        update(nextAvailableTaskTime, idleCount);
    }

    void TaskQueue::update(std::chrono::steady_clock::time_point &nextAvailableTaskTime, int idleCount)
    {
        while (std::optional<Threading::TaskTracker> f = fetch(nextAvailableTaskTime, idleCount)) {
            f->mTask();
        }
    }

    void TaskQueue::waitForTasks(std::chrono::steady_clock::time_point until)
    {
        std::unique_lock<std::mutex> lock(mMutex);
        auto cond = [this]() { return !mQueue.empty() || !mRunning; };
        if (until == std::chrono::steady_clock::time_point::max()) {
            mCv.wait(lock, cond);
        } else {
            mCv.wait_until(lock, until, cond);
        }
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

    std::optional<TaskTracker> TaskQueue::fetch(std::chrono::steady_clock::time_point &nextTask, int &idleCount)
    {
        std::chrono::steady_clock::time_point nextTaskTimepoint = std::chrono::steady_clock::time_point::max();
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
            if (mRunning) {
                for (RepeatedTask &task : mRepeatedTasks) {
                    if (task.mNextExecuted <= std::chrono::steady_clock::now()) {
                        task.mNextExecuted = std::chrono::steady_clock::now() + task.mInterval;
                        return wrapTask([&]() { task.mTask(); });
                    } else {
                        nextTaskTimepoint = std::min(task.mNextExecuted, nextTaskTimepoint);
                    }
                }
            }
        }

        if (mRunning && idleCount != 0) {
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
        nextTask = nextTaskTimepoint;
        return {};
    }

    bool TaskQueue::idle() const
    {
        return mTaskCount == 0;
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