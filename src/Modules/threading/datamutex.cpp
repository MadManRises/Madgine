#include "../moduleslib.h"

#include "datamutex.h"

#include "Generic/execution/execution.h"

#include "taskqueue.h"

namespace Engine {
namespace Threading {

    DataMutex::Lock::Lock(DataMutex &mutex, AccessMode mode
#if MODULES_ENABLE_TASK_TRACKING
        ,
        Threading::TaskQueue *queue
#endif
        )
        : mMutex(&mutex)
        , mMode(mode)
#if MODULES_ENABLE_TASK_TRACKING
        , mQueue(queue)
#endif
    {
    }

    DataMutex::Lock::Lock(Lock &&other)
        : mMutex(std::exchange(other.mMutex, nullptr))
        , mMode(other.mMode)
#if MODULES_ENABLE_TASK_TRACKING
        , mQueue(std::exchange(other.mQueue, nullptr))
#endif
    {
    }

    DataMutex::Lock::~Lock()
    {
        if (mMutex)
            mMutex->unlockImpl(mMode
#if MODULES_ENABLE_TASK_TRACKING
                ,
                mQueue
#endif
            );
    }

    DataMutex::Lock &DataMutex::Lock::operator=(Lock &&other)
    {
        std::swap(mMutex, other.mMutex);
        std::swap(mMode, other.mMode);
#if MODULES_ENABLE_TASK_TRACKING
        std::swap(mQueue, other.mQueue);
#endif
        return *this;
    }

    bool DataMutex::Lock::isHeld() const
    {
        return mMutex;
    }

    DataMutex::Awaiter::Awaiter(DataMutex *mutex, AccessMode mode)
        : mMutex(mutex)
        , mMode(mode)
    {
    }

    bool DataMutex::Awaiter::await_ready() noexcept
    {
        return false;
    }

    bool DataMutex::Awaiter::await_suspend(TaskHandle task)
    {
        if (tryLock(task.queue())) {
            task.release();
            return false;
        } else {
            task.queue()->queueHandle(std::move(task), this);
            return true;
        }
    }

    DataMutex::Lock DataMutex::Awaiter::await_resume()
    {
        return std::move(mLock);
    }

    DataMutex::Moded::Moded(DataMutex *mutex, AccessMode mode)
        : mMutex(mutex)
        , mMode(mode)
    {
    }

    DataMutex::Lock DataMutex::Moded::lock()
    {
        return mMutex->lock(mMode);
    }

    DataMutex::Lock DataMutex::Moded::tryLock(TaskQueue *queue)
    {
        return mMutex->tryLock(queue, mMode);
    }

    DataMutex::Awaiter DataMutex::Moded::operator co_await()
    {
        return { mMutex, mMode };
    }

    bool DataMutex::Awaiter::tryLock(TaskQueue *queue)
    {
        assert(!mLock.isHeld());
        mLock = mMutex->tryLock(queue, mMode);
        return mLock.isHeld();
    }

    DataMutex::DataMutex(std::string_view name)
        : mName(name)
    {
    }

    DataMutex::Moded DataMutex::operator()(AccessMode mode)
    {
        return { this, mode };
    }

    const std::string &DataMutex::name() const
    {
        return mName;
    }

    DataMutex::Lock DataMutex::lock(AccessMode mode)
    {
        switch (mode) {
        case AccessMode::READ:
            mMutex.lock_shared();
            break;
        case AccessMode::WRITE:
            mMutex.lock();
            break;
        default:
            throw 0;
        }
        return { *this, mode };
    }

    DataMutex::Lock DataMutex::tryLock(TaskQueue *queue, AccessMode mode)
    {
        bool success;
        switch (mode) {
        case AccessMode::READ:
            success = mMutex.try_lock_shared();
            break;
        case AccessMode::WRITE:
            success = mMutex.try_lock();
            break;
        default:
            throw 0;
        }
        if (success) {
#if MODULES_ENABLE_TASK_TRACKING
            Debug::Threading::onLock(this, mode, queue);
            return { *this, mode, queue };
#else
            return { *this, mode };
#endif
        } else
            return {};
    }

    void DataMutex::unlockImpl(AccessMode mode
#if MODULES_ENABLE_TASK_TRACKING
        ,
        TaskQueue *queue
#endif
    )
    {
#if MODULES_ENABLE_TASK_TRACKING
        if (queue)
            Debug::Threading::onUnlock(this, mode, queue);
#endif
        switch (mode) {
        case AccessMode::READ:
            return mMutex.unlock_shared();
        case AccessMode::WRITE:
            return mMutex.unlock();
        default:
            throw 0;
        }
    }

}
}
