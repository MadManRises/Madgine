#pragma once

#include "Generic/coroutines/handle.h"
#include "taskfuture.h"

namespace Engine {
namespace Threading {

    template <typename F, typename... Args>
    auto make_task(F f, Args &&...args)
    {
        static_assert(!std::is_reference_v<F>);

        using R = std::invoke_result_t<F, Args...>;

        if constexpr (is_instance_v<R, Task>) {
            return f(std::forward<Args>(args)...);
        } else {
            return [](F f, Args... args) -> Task<std::invoke_result_t<F, Args...>> {
                co_return std::move(f)(std::forward<Args>(args)...);
            }(std::move(f), std::forward<Args>(args)...);
        }
    }

    template <typename T, typename Immediate>
    struct Task {

        static_assert(!is_instance_v<T, Task>);

        struct promise_type : TaskPromise<T> {
            Task<T, Immediate> get_return_object()
            {
                return { CoroutineHandle<promise_type>::fromPromise(*this) };
            }
        };

        template <typename T2, typename I>
        friend struct Task;

        template <typename I>
        Task(Task<T, I> &&other)
            : mHandle(std::move(other.mHandle))
            , mState(std::move(other.mState))
            , mImmediate(other.mImmediate)
        {
        }

        Task(std::shared_ptr<TaskPromiseSharedState<T>> state)
            : mState(std::move(state))
        {
        }

        Task(CoroutineHandle<promise_type> handle)
            : mHandle(std::move(handle))
        {
        }

        TaskFuture<T> get_future() {
            if (!mState) {
                mState = mHandle->get_state();
            }
            return mState;
        }

        TaskHandle assign(TaskQueue *queue) &&
        {
            if (mHandle) {
                mHandle->setQueue(queue);
            }
            TaskHandle handle { std::move(mHandle) };
            if (mImmediate)
                handle();
            return { std::move(handle) };
        }

    private:
        CoroutineHandle<TaskPromise<T>> mHandle;
        std::shared_ptr<TaskPromiseSharedState<T>> mState;
        bool mImmediate = Immediate {};
    };

    template <typename T>
    using ImmediateTask = Task<T, std::true_type>;

    template <typename T>
    Task<T> make_ready_task(T &&val)
    {
        return { std::make_shared<TaskPromiseSharedState<T>>(std::forward<T>(val)) };
    }

}
}