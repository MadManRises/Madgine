#include "../moduleslib.h"

#include "taskhandle.h"
#include "taskpromise.h"
#include "taskqueue.h"

namespace Engine {
namespace Threading {

    TaskHandle::TaskHandle(CoroutineHandle<TaskPromiseTypeBase> handle)
        : mHandle(handle.release())
    {
    }

    TaskHandle::TaskHandle(TaskHandle &&other)
        : mHandle(std::exchange(other.mHandle, {}))
    {
    }

    TaskHandle &TaskHandle::operator=(TaskHandle &&other)
    {
        std::swap(mHandle, other.mHandle);
        return *this;
    }

    TaskHandle::~TaskHandle()
    {
        assert(!mHandle);
    }

    void TaskHandle::operator()()
    {
        mHandle.resume();
        mHandle = {};
    }

    void TaskHandle::resumeInQueue()
    {
        queue()->queueHandle(std::move(*this));
    }

    std::coroutine_handle<TaskPromiseTypeBase> TaskHandle::release()
    {
        return std::exchange(mHandle, std::coroutine_handle<TaskPromiseTypeBase> {});
    }

    TaskQueue *TaskHandle::queue() const
    {
        return mHandle.promise().queue();
    }

    TaskHandle::operator bool() const
    {
        return static_cast<bool>(mHandle);
    }

}
}