#include "../moduleslib.h"

#if ENABLE_THREADING
#    include "Interfaces/threading/threadapi.h"
#    include "threadlocal.h"
#    include "threadstorage.h"
#endif

#include "workgroup.h"
#include "workgroupstorage.h"

#include "barrier.h"
#include "task.h"
#include "taskqueue.h"

namespace Engine {
namespace Threading {

#if ENABLE_THREADING
    static THREADLOCAL(WorkGroup *) sSelf = nullptr;
#else
    static WorkGroup *sSelf = nullptr;
#endif

    static std::atomic<size_t> sWorkgroupInstanceCounter = 0;

    static std::vector<std::function<void()>> &sThreadInitializers()
    {
        static std::vector<std::function<void()>> dummy;
        return dummy;
    }

    WorkGroup::WorkGroup(const std::string &name)
        : mInstanceCounter(sWorkgroupInstanceCounter.fetch_add(1))
        , mName(name.empty() ? "Workgroup_" + std::to_string(mInstanceCounter) : name)
    {
#if ENABLE_THREADING
        ThreadStorage::init(true);
        ThreadStorage::init(false);

        setCurrentThreadName(mName + "_Main");
#endif

        assert(!sSelf);
        sSelf = this;

        WorkGroupStorage::init(true);
        WorkGroupStorage::init(false);

        for (const std::function<void()> &task : sThreadInitializers()) {
            task();
        }

        //Is that useful at all?
        for (const std::function<void()> &task : mThreadInitializers) {
            task();
        }
    }

    WorkGroup::~WorkGroup()
    {
        WorkGroupStorage::finalize(false);
        WorkGroupStorage::finalize(true);

        assert(sSelf == this);
        sSelf = nullptr;

#if ENABLE_THREADING
        ThreadStorage::finalize(false);
        ThreadStorage::finalize(true);

        assert(singleThreaded());
#endif
    }

    void WorkGroup::addThreadInitializer(std::function<void()> &&task)
    {
#if ENABLE_THREADING
        assert(mSubThreads.empty());
#endif
        mThreadInitializers.emplace_back(std::move(task));
    }

    void WorkGroup::addStaticThreadInitializer(std::function<void()> &&task)
    {
        sThreadInitializers().emplace_back(std::move(task));
    }

    const std::string &WorkGroup::name() const
    {
        return mName;
    }

#if ENABLE_THREADING
    bool WorkGroup::singleThreaded()
    {
        return mSubThreads.empty();
    }

    void WorkGroup::checkThreadStates()
    {

        auto pivot = std::remove_if(mSubThreads.begin(), mSubThreads.end(),
            [](Future<int> &f) {
                bool result = f.is_ready();
                if (result)
                    f.get();
                return result;
            });

        mSubThreads.erase(pivot, mSubThreads.end());
    }

    void WorkGroup::initThread()
    {

        ThreadStorage::init(true);
        ThreadStorage::init(false);

        assert(!sSelf);
        sSelf = this;

        for (const std::function<void()> &task : sThreadInitializers()) {
            task();
        }

        for (const std::function<void()> &task : mThreadInitializers) {
            task();
        }
    }

    void WorkGroup::finalizeThread()
    {

        assert(sSelf == this);
        sSelf = nullptr;

        ThreadStorage::finalize(false);
        ThreadStorage::finalize(true);
    }

    void WorkGroup::addThread()
    {
        std::lock_guard lock { mMutex };
        for (Barrier &barrier : mBarriers) {
            if (barrier.running())
                barrier.addThread();
        }
    }
#endif

    WorkGroup &WorkGroup::self()
    {
        assert(isInitialized());
        return *sSelf;
    }

    bool WorkGroup::isInitialized()
    {
#if ENABLE_THREADING && USE_CUSTOM_THREADLOCAL
        assert(sSelf.valid());
#endif
        return sSelf != nullptr;
    }

    void WorkGroup::addTaskQueue(TaskQueue *taskQueue)
    {
        mTaskQueues.push_back(taskQueue);
    }

    void WorkGroup::removeTaskQueue(TaskQueue *taskQueue)
    {
        mTaskQueues.erase(std::find(mTaskQueues.begin(), mTaskQueues.end(), taskQueue));
    }

    const std::vector<TaskQueue *> WorkGroup::taskQueues() const
    {
        return mTaskQueues;
    }

    Barrier &WorkGroup::barrier(int flags)
    {
        WorkGroup &group = self();
        std::lock_guard lock { group.mMutex };
        Barrier &barrier = group.mBarriers.emplace_back(flags,
#if ENABLE_THREADING
            group.mSubThreads.size() +
#endif
                1);
        if (!group.mHasInterrupt) {
            group.mHasInterrupt = true;
            for (TaskQueue *queue : group.mTaskQueues)
                queue->notify();
        }
        return barrier;
    }

    void WorkGroup::enterCurrentBarrier(TaskQueue *queue, size_t queueIndex, bool isMain)
    {
        std::list<Barrier>::iterator it;
        {
            std::lock_guard lock { mMutex };
            if (mBarriers.empty())
                return;
            it = mBarriers.begin();
            while (it->started()) {
                ++it;
                if (it == mBarriers.end())
                    return;
            }
        }
        it->enter(queue, queueIndex, isMain);
        if (isMain) {
            std::lock_guard lock { mMutex };
            mHasInterrupt = mBarriers.erase(it) != mBarriers.end();
        }
    }

    const std::atomic<bool> &WorkGroup::hasInterrupt()
    {
        return mHasInterrupt;
    }

}
}