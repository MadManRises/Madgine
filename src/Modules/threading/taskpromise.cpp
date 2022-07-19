#include "../moduleslib.h"

#include "taskpromise.h"

#if ENABLE_TASK_TRACKING
#    include "Interfaces/debug/stacktrace.h"
#    include "taskqueue.h"
#endif

namespace Engine {
namespace Threading {

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

    bool TaskPromiseSharedStateBase::then(TaskHandle handle)
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

    void TaskPromiseTypeBase::setQueue(TaskQueue *queue)
    {
        assert(queue);
        assert(!mQueue);
        mQueue = queue;
    }

    TaskQueue *TaskPromiseTypeBase::queue() const
    {
        return mQueue;
    }

    void TaskHandleInitialSuspend::await_resume() noexcept
    {
#if ENABLE_TASK_TRACKING
        Debug::Threading::onAssign(std::coroutine_handle<TaskPromiseTypeBase>::from_promise(*mPromise), mPromise->queue(), Debug::StackTrace<1>::getCurrent(1));
#endif
    }

}
}