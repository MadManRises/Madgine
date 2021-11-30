#pragma once

#include "taskhandle.h"
#include "taskpromise.h"

namespace Engine {
namespace Threading {

    template <typename F, typename... Args>
    auto make_task(F f, Args &&...args);

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
        
        void reset()
        {
            mState.reset();
        }

        bool is_ready() const
        {
            return mState->is_ready();
        }

        bool await_ready() const
        {
            return is_ready();
        }

        void await_suspend(TaskHandle handle)
        {
            mState->then(std::move(handle));
        }

        const T &await_resume() const
        {
            return get();
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

        const T &get() const
        {
            assert(is_ready());
            return mState->get();
        }

        operator const T &() const
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

        void reset()
        {
            mState.reset();
        }

        bool is_ready() const
        {
            return !mState || mState->is_ready();
        }

        bool await_ready() const
        {
            return is_ready();
        }

        void await_suspend(TaskHandle handle)
        {
            mState->then(std::move(handle));
        }

        void await_resume() const
        {
            assert(is_ready());
        }

        template <typename F>
        auto then(F &&f, Threading::TaskQueue *queue)
        {
            auto task = make_task(std::forward<F>(f));
            auto fut = task.get_future();
            auto handle = std::move(task).assign(queue);
            mState->then(std::move(handle));
            return fut;
        }

        std::shared_ptr<TaskPromiseSharedState<void>> mState;
    };

}
}