#include "../moduleslib.h"

#include "taskpromise.h"
#include "taskqueue.h"
#include "barrier.h"

namespace Engine {
namespace Threading {

    void TaskPromiseSharedStateBase::finalize()
    {
        std::lock_guard guard { mMutex };
        for (CoroutineHandle<TaskPromiseTypeBase> &handle : mThenResumes)
            handle->resume(std::move(handle));
        mThenResumes.clear();
    }

    void TaskPromiseSharedStateBase::notifyDestroyed()
    {
        std::lock_guard guard { mMutex };
        mDestroyed = true;
    }

    void TaskPromiseTypeBase::resume(CoroutineHandle<TaskPromiseTypeBase> handle)
    {
        if (mState->mBarrier)
            mState->mBarrier->queueHandle(mState->mQueue, std::move(handle));
        else
            mState->mQueue->queueHandle(std::move(handle), TaskMask::ALL);
    }

    void TaskPromiseTypeBase::setQueue(TaskQueue* queue, Barrier* barrier) {
        mState->mQueue = queue;
        mState->mBarrier = barrier;
    }

}
}