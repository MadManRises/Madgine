#include "../moduleslib.h"

#include "taskpromise.h"
#include "taskqueue.h"
#include "barrier.h"

namespace Engine {
namespace Threading {

    void TaskPromiseSharedStateBase::finalize()
    {
        std::lock_guard guard { mMutex };
        for (TaskHandle &handle : mThenResumes)
            handle.resumeInQueue();
        mThenResumes.clear();
    }

    void TaskPromiseSharedStateBase::notifyDestroyed()
    {
        std::lock_guard guard { mMutex };
        mDestroyed = true;
    }

    void TaskPromiseTypeBase::resume(TaskHandle handle)
    {
        if (mBarrier)
            mBarrier->queueHandle(mQueue, std::move(handle));
        else
            mQueue->queueHandle(std::move(handle), TaskMask::ALL);
    }

    TaskQueue* TaskPromiseTypeBase::queue() const {
        return mQueue;
    }

}
}