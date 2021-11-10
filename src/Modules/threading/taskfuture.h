#pragma once

#include "taskpromise.h"

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
            mState->then_resume(std::move(handle));
        }

        template <typename F>
        auto then(F &&f)
        {
            /* auto [fut, handle] = make_task(std::forward<F>(f), *this).release();
            handle->setQueue(mState->mQueue, mState->mBarrier);
            mState->then_resume(std::move(handle));
            return std::move(fut);
            */
            LOG_ONCE("Todo!");
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
            mState->then_resume(std::move(handle));
        }

        void await_resume()
        {
            assert(is_ready());
        }

        std::shared_ptr<TaskPromiseSharedState<void>> mState;
    };

}
}