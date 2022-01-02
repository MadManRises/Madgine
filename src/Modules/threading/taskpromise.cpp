#include "../moduleslib.h"

#include "taskpromise.h"

#if ENABLE_TASK_TRACKING
#    include "taskqueue.h"
#include "Interfaces/debug/stacktrace.h"
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
    }

    void TaskPromiseSharedStateBase::notifyDestroyed()
    {
        std::lock_guard guard { mMutex };
        mDestroyed = true;
    }

    void TaskPromiseSharedStateBase::then(TaskHandle handle)
    {
        std::lock_guard guard { mMutex };
        mThenResumes.emplace_back(std::move(handle));
    }

    void TaskPromiseTypeBase::setQueue(TaskQueue *queue)
    {
        assert(queue);
        assert(!mQueue);
        mQueue = queue;

#if ENABLE_TASK_TRACKING
        queue->mTracker.onAssign(std::coroutine_handle<TaskPromiseTypeBase>::from_promise(*this).address(), Debug::StackTrace<2>::getCurrent(2));
#endif
    }

    TaskQueue *TaskPromiseTypeBase::queue() const
    {
        return mQueue;
    }

}
}