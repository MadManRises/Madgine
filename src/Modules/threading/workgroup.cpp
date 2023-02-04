#include "../moduleslib.h"

#if ENABLE_THREADING
#    include "Interfaces/threading/threadapi.h"
#    include "threadlocal.h"
#    include "threadstorage.h"
#endif

#include "workgroup.h"
#include "workgroupstorage.h"

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

    WorkGroup::WorkGroup(std::string_view name)
        : mInstanceCounter(sWorkgroupInstanceCounter++)
        , mName(name.empty() ? "Workgroup_" + std::to_string(mInstanceCounter) : name)
    {
#if ENABLE_THREADING
        mThreads.push_back(std::this_thread::get_id());

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
        {
            std::unique_lock lock { mThreadsMutex };
            assert(mSubThreads.empty());
        }
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
        std::unique_lock lock { mThreadsMutex };
        std::erase_if(mSubThreads,
            [](std::future<int> &f) {
                bool result = f.wait_for(0ms) != std::future_status::timeout;
                if (result)
                    f.get();
                return result;
            });
    }

    bool WorkGroup::contains(std::thread::id id) const
    {
        std::unique_lock lock { mThreadsMutex };
        return std::ranges::find(mThreads, id) != mThreads.end();
    }

    void WorkGroup::initThread()
    {
        {
            std::unique_lock lock { mThreadsMutex };
            mThreads.push_back(std::this_thread::get_id());
        }

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
        std::erase(mTaskQueues, taskQueue);
    }

    const std::vector<TaskQueue *> &WorkGroup::taskQueues() const
    {
        return mTaskQueues;
    }

}
}