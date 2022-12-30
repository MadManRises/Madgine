#include "../moduleslib.h"

#include "taskpromisesharedstate.h"

#include "taskhandle.h"

namespace Engine {
namespace Threading {

    TaskPromiseSharedStateBase::TaskPromiseSharedStateBase(bool ready)
        : mAttached(ready)
        , mDestroyed(ready)
        , mDone(ready)
    {
    }

    TaskPromiseSharedStateBase::~TaskPromiseSharedStateBase()
    {
    }

    void TaskPromiseSharedStateBase::attach()
    {
        std::lock_guard guard { mMutex };
        assert(!mAttached);
        mAttached = true;
    }

    void TaskPromiseSharedStateBase::finalize()
    {
        std::lock_guard guard { mMutex };
        for (TaskHandle &handle : mThenResumes) {
            handle.resumeInQueue();
        }
        mThenResumes.clear();
        mDone = true;
    }

    void TaskPromiseSharedStateBase::notifyDestroyed()
    {
        std::lock_guard guard { mMutex };
        mDestroyed = true;
    }

    bool TaskPromiseSharedStateBase::then_await(TaskHandle handle)
    {
        std::lock_guard guard { mMutex };
        if (!mDone) {
            mThenResumes.emplace_back(std::move(handle));
            return true;
        } else {
            handle.release();
            return false;
        }
    }

    void TaskPromiseSharedStateBase::then(TaskHandle handle)
    {
        std::lock_guard guard { mMutex };
        if (!mDone) {
            mThenResumes.emplace_back(std::move(handle));
        } else {
            handle.resumeInQueue();
        }
    }

    void TaskPromiseSharedStateBase::setException(std::exception_ptr exception)
    {
        std::lock_guard guard { mMutex };
        mException = std::move(exception);
    }

    void TaskPromiseSharedStateBase::rethrowIfException()
    {
        if (mException) {
            std::rethrow_exception(mException);
        }
    }

}
}