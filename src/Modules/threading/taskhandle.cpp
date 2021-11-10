#include "../moduleslib.h"

#include "taskhandle.h"
#include "taskpromise.h"

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
        mHandle = std::exchange(other.mHandle, {});
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
        mHandle.promise().resume(std::move(*this));
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