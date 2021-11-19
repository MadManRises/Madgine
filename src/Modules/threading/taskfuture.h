#pragma once

#include "taskpromise.h"
#include "taskhandle.h"

namespace Engine {
namespace Threading {

    template <typename T>
    struct TaskFuture {

        TaskFuture() = default;

        TaskFuture(T val)
            : mState(std::make_shared<TaskPromiseSharedState<T>>(std::move(val)))
        {
        }

        TaskFuture(std::shared_ptr<TaskPromiseSharedState<T>> state)
            : mState(std::move(state))
        {
        }

        TaskFuture(const TaskFuture &) = default;
        TaskFuture(TaskFuture &&) = default;

        TaskFuture &operator=(const TaskFuture &) = default;
        TaskFuture &operator=(TaskFuture &&) = default;

        bool valid()
        {
            return mState && mState->valid();
        }

        bool is_ready()
        {
            return mState->is_ready();
        }

        bool await_ready()
        {
            return is_ready();
        }

        void await_suspend(TaskHandle handle)
        {
            mState->then(std::move(handle));
        }

        template <typename F>
        auto then(F &&f, Threading::TaskQueue *queue)
        {
            auto task = make_task(std::forward<F>(f), TaskFuture<T> { *this });
            auto fut = task.get_future();
            auto handle = std::move(task).assign(queue);
            mState->then(std::move(handle));
            return fut;
        }

        T &await_resume()
        {
            return get();
        }

        T &get()
        {
            assert(is_ready());
            return mState->get();
        }

        operator T &()
        {
            return get();
        }

        std::shared_ptr<TaskPromiseSharedState<T>> mState;
    };

    template <>
    struct TaskFuture<void> {

        TaskFuture() = default;

        TaskFuture(std::shared_ptr<TaskPromiseSharedState<void>> state)
            : mState(std::move(state))
        {
        }

        bool valid()
        {
            return mState && mState->valid();
        }

        bool is_ready()
        {
            return !mState || mState->is_ready();
        }

        bool await_ready()
        {
            return is_ready();
        }

        void await_suspend(TaskHandle handle)
        {
            mState->then(std::move(handle));
        }

        void await_resume()
        {
            assert(is_ready());
        }

        std::shared_ptr<TaskPromiseSharedState<void>> mState;
    };

}
}