#include "../moduleslib.h"

#include "task.h"

namespace Engine {
namespace Threading {

    TaskHandle::TaskHandle(CoroutineHandle<TaskPromiseTypeBase> handle)
        : mHandle(std::move(handle))
    {
    }

    TaskHandle::~TaskHandle()
    {
        assert(!mHandle);
    }

    void TaskHandle::reset()
    {
        mHandle.reset();
    }

    CoroutineHandle<TaskPromiseTypeBase> TaskHandle::release()
    {
        return std::move(mHandle);
    }

    void TaskHandle::setQueue(TaskQueue *queue, Barrier *barrier)
    {
        mHandle->setQueue(queue, barrier);
    }

    TaskState TaskHandle::operator()()
    {
        mHandle.resume();
        if (mHandle.done()) {
            reset();
            return RETURN;
        }
        mHandle.release();
        return SUSPENDED;
    }

    TaskHandle::operator bool() const
    {
        return static_cast<bool>(mHandle);
    }

}
}