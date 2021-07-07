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
            //LOG(std::this_thread::get_id() << " arrived");
            if (isMain) {
                size_t threadAcc = mThreadAccumulator;
                while (threadAcc < mThreadCount) {
                    mThreadAccumulator.wait(threadAcc);
                    threadAcc = mThreadAccumulator;
                }
                bool done = true;
                for (TaskQueue *q : WorkGroup::self().taskQueues()) {
                    if (!q->idle(TaskMask::BARRIER)) {
                        done = false;
                    }
                }
                mThreadAccumulator = 0;
                if (done) {
                    mState = State::RUN;
                    std::unique_lock lock { mMutex };
                    for (std::pair<TaskQueue *const, std::vector<TaskHandle>> &tasks : mTaskLists) {
                        for (TaskHandle &task : tasks.second)
                            tasks.first->queue(std::move(task), TaskMask::BARRIER);
                    }
                    mTaskLists.clear();
                    //LOG(std::this_thread::get_id() << " -> RUN");
                    mState.notify_all();
                }
            } else {
                mThreadAccumulator.notify_one();
                mState.wait(State::INIT);
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

            //LOG(std::this_thread::get_id() << " ran");
            ++mThreadAccumulator;
            if (isMain) {
                size_t threadAcc = mThreadAccumulator;
                while (threadAcc < mThreadCount) {
                    mThreadAccumulator.wait(threadAcc);
                    threadAcc = mThreadAccumulator;
                }
                bool done = true;
                for (TaskQueue *q : WorkGroup::self().taskQueues()) {
                    if (!q->idle(TaskMask::BARRIER)) {
                        done = false;
                    }
                }
                mThreadAccumulator = 0;
                if (done) {
                    //LOG(std::this_thread::get_id() << " -> DONE");
                    mState = State::DONE;
                    mState.notify_all();
                }
            } else {
                mThreadAccumulator.notify_one();
                mState.wait(State::RUN);
            }
        }

        //LOG(std::this_thread::get_id() << " out");
        ++mThreadAccumulator;
        if (isMain) {
            size_t threadAcc = mThreadAccumulator;
            while (threadAcc < mThreadCount) {
                mThreadAccumulator.wait(threadAcc);
                threadAcc = mThreadAccumulator;                
            }
            //LOG(std::this_thread::get_id() << " -> EXIT");
        } else {
            mThreadAccumulator.notify_one();
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