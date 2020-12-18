#include "../moduleslib.h"

#include "barrier.h"

#include "taskqueue.h"

#include "workgroup.h"

namespace Engine {
namespace Threading {
    Barrier::Barrier(int flags, size_t threadCount)
        : mFlags(flags)
        , mThreadCount(threadCount)
    {
    }
    void Barrier::enter(TaskQueue *queue, size_t queueIndex, bool isMain)
    {
        assert(mState != State::DONE);

        while (mState == State::INIT) {
            while (!queue->idle(TaskMask::BARRIER)) {
                queue->update(TaskMask::BARRIER, nullptr, 0, 1);
            }
            ++mThreadAccumulator;
            if (isMain) {
                if (mThreadAccumulator < mThreadCount) {
                    std::unique_lock lock { mMutex };
                    mCv_main.wait(lock, [this]() { return mThreadAccumulator == mThreadCount; });
                }
                bool done = true;
                for (TaskQueue *q : WorkGroup::self().taskQueues()) {
                    if (!q->idle(TaskMask::BARRIER)) {
                        done = false;
                    }
                }
                mThreadAccumulator = 0;
                if (done) {
                    std::unique_lock lock { mMutex };
                    for (std::pair<TaskQueue *const, std::vector<TaskHandle>> &tasks : mTaskLists) {
                        for (TaskHandle &task : tasks.second)
                            tasks.first->queue(std::move(task), TaskMask::BARRIER);
                    }
                    mTaskLists.clear();
                    mState = State::RUN;
                }
                mCv.notify_all();
            } else {
                std::unique_lock lock { mMutex };
                mCv_main.notify_one();
                if (mState == State::INIT)
                    mCv.wait(lock);
            }
        }

        while (mState == State::RUN) {
            bool running = false;
            do {
                queue->update(TaskMask::BARRIER, nullptr, 0, 1);
                if (isMain || (mFlags & RUN_ONLY_ON_MAIN_THREAD) == 0) {
                    TaskHandle task;
                    {
                        std::lock_guard lock { mMutex };
                        running = !mTasks.empty();
                        if (running) {
                            task = std::move(mTasks.front());
                            mTasks.erase(mTasks.begin());
                        }
                    }
                    if (task) {
                        TaskState state = task();
                        if (state == YIELD) {
                            std::lock_guard lock { mMutex };
                            mTasks.emplace_back(std::move(task));
                        }
                    }
                }
            } while (running);

            ++mThreadAccumulator;
            if (isMain) {
                if (mThreadAccumulator < mThreadCount) {
                    std::unique_lock lock { mMutex };
                    mCv_main.wait(lock, [this]() { return mThreadAccumulator == mThreadCount; });
                }
                bool done = true;
                for (TaskQueue *q : WorkGroup::self().taskQueues()) {
                    if (!q->idle(TaskMask::BARRIER)) {
                        done = false;
                    }
                }
                mThreadAccumulator = 0;
                if (done) {
                    std::unique_lock lock { mMutex };
                    mState = State::DONE;
                }
                mCv.notify_all();
            } else {
                mCv_main.notify_one();
                std::unique_lock lock { mMutex };
                if (mState == State::RUN)
                    mCv.wait(lock);
            }
        }

        ++mThreadAccumulator;
        if (isMain) {
            if (mThreadAccumulator < mThreadCount) {
                std::unique_lock lock { mMutex };
                mCv_main.wait(lock, [this]() { return mThreadAccumulator == mThreadCount; });
            }
        } else {
            mCv_main.notify_one();
        }
    }

    void Barrier::queue(TaskQueue *queue, TaskHandle task)
    {
        assert(mState != State::DONE);
        if (!queue) {
            std::lock_guard lock { mMutex };
            mTasks.emplace_back(std::move(task));
        } else if (mState == State::INIT) {
            std::lock_guard lock { mMutex };
            mTaskLists[queue].emplace_back(std::move(task));
        } else {
            queue->queue(std::move(task), TaskMask::BARRIER);
        }
    }

    bool Barrier::running()
    {
        return mState != State::DONE;
    }

    bool Barrier::started()
    {
        return mState != State::INIT;
    }

    void Barrier::addThread()
    {
        std::unique_lock lock { mMutex };
        ++mThreadCount;
    }

}
}