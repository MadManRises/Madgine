#pragma once

namespace Engine {
namespace Threading {

    struct MODULES_EXPORT TaskHandle {

        TaskHandle() = default;
        TaskHandle(const TaskHandle &) = delete;
        TaskHandle(TaskHandle &&other);
        
        TaskHandle &operator=(const TaskHandle &) = delete;
        TaskHandle &operator=(TaskHandle &&);

        TaskHandle(CoroutineHandle<TaskPromiseTypeBase> handle);
        template <typename T>
        TaskHandle(std::coroutine_handle<T> handle)
            : mHandle(std::coroutine_handle<TaskPromiseTypeBase>::from_promise(handle.promise()))
        {
        }
        ~TaskHandle();

        void operator()();
        void resumeInQueue();

        TaskQueue *queue() const;

        explicit operator bool() const;


    private:
        std::coroutine_handle<TaskPromiseTypeBase> mHandle;
    };

}
}