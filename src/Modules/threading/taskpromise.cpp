#include "../moduleslib.h"

#include "taskpromise.h"
#include "taskqueue.h"

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

    void TaskPromiseSharedStateBase::then(TaskHandle handle)
    {
        std::lock_guard guard { mMutex };
        mThenResumes.emplace_back(std::move(handle));
    }

    void TaskPromiseTypeBase::resume(TaskHandle handle)
    {
        mQueue->queueHandle(std::move(handle));
    }

    void TaskPromiseTypeBase::setQueue(TaskQueue* queue) {
        assert(queue);
        assert(!mQueue);
        mQueue = queue;
    }

    TaskQueue *TaskPromiseTypeBase::queue() const
    {
        return mQueue;
    }

}
}