#pragma once

#include "Generic/coroutines/handle.h"
#include "taskfuture.h"

namespace Engine {
namespace Threading {

    enum TaskState {
        RETURN,
        SUSPENDED
    };

    template <typename F, typename... Args>
    auto make_task(F f, Args &&...args)
    {
        static_assert(!std::is_reference_v<F>);
        if constexpr (is_instance_v<std::invoke_result_t<F, Args...>, Task>) {
            return f(std::forward<Args>(args)...);
        } else {
            return [](F f, Args... args) -> Task<std::invoke_result_t<F, Args...>> {
                co_return std::move(f)(std::forward<Args>(args)...);
            }(std::move(f), std::forward<Args>(args)...);
        }
    }

    struct MODULES_EXPORT TaskHandle {

        TaskHandle() = default;
        TaskHandle(TaskHandle &&other) = default;
        TaskHandle(const TaskHandle &) = delete;
        TaskHandle(TaskHandle &) = delete;

        TaskHandle &operator=(TaskHandle &&) = default;

        TaskHandle(CoroutineHandle<TaskPromiseTypeBase> handle);
        ~TaskHandle();

        void reset();
        CoroutineHandle<TaskPromiseTypeBase> release();

        void setQueue(TaskQueue *queue, Barrier *barrier = nullptr);

        TaskState operator()();

        explicit operator bool() const;

    protected:
        CoroutineHandle<TaskPromiseTypeBase> mHandle;
    };

    template <typename T>
    struct Task : TaskHandle {

        static_assert(!is_instance_v<T, Task>);

        struct promise_type : TaskPromiseTypeBase {

            promise_type()
                : TaskPromiseTypeBase(std::make_shared<TaskPromiseSharedState<T>>())
            {
            }

            Task<T> get_return_object()
            {
                return { CoroutineHandle<promise_type>::fromPromise(*this) };
            }

            void return_value(T value) noexcept
            {
                static_cast<TaskPromiseSharedState<T> *>(mState.get())->set_value(std::move(value));                
            }
        };

        Task(CoroutineHandle<promise_type> handle)
            : TaskHandle(std::move(handle))
        {
        }

        std::pair<TaskFuture<T>, CoroutineHandle<TaskPromiseTypeBase>> release() &&
        {
            return { std::static_pointer_cast<TaskPromiseSharedState<T>>(mHandle->mState), std::move(mHandle) };
        }

    };

    template <>
    struct Task<void> : TaskHandle {

        struct promise_type : TaskPromiseTypeBase {

            promise_type()
                : TaskPromiseTypeBase(std::make_shared<TaskPromiseSharedState<void>>())
            {
            }

            Task<void> get_return_object()
            {
                return {
                    CoroutineHandle<promise_type>::fromPromise(*this)
                };
            }

            void return_void()
            {
                static_cast<TaskPromiseSharedState<void> *>(mState.get())->set_value();                
            }
        };

        Task(CoroutineHandle<promise_type> handle)
            : TaskHandle(std::move(handle))
        {
        }

        std::pair<TaskFuture<void>, CoroutineHandle<TaskPromiseTypeBase>> release() &&
        {
            return { std::static_pointer_cast<TaskPromiseSharedState<void>>(mHandle->mState), std::move(mHandle) };
        }
    };

}
}