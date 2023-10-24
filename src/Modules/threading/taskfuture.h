#pragma once

#include "taskhandle.h"
#include "taskpromisesharedstate.h"

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

        bool valid() const
        {
            return static_cast<bool>(mState);
        }

        bool attached() const
        {
            return mState && mState->mAttached;
        }

        void reset()
        {
            mState.reset();
        }

        std::shared_ptr<TaskPromiseSharedState<T>> release()
        {
            return std::move(mState);
        }

        bool is_ready() const
        {
            return mState->is_ready();
        }

        bool await_ready() const
        {
            return is_ready();
        }

        bool await_suspend(TaskHandle handle)
        {
            return mState->then_await(std::move(handle));
        }

        const T &await_resume() const
        {
            return get();
        }

        template <typename F>
        auto then(F &&f, TaskQueue *queue)
        {
            auto task = [](F f, TaskFuture<T> fut) -> decltype(make_task(std::forward<F>(f), fut)) {
                co_return co_await make_task(std::forward<F>(f), fut);
            }(std::forward<F>(f), *this);

            return then_task(std::move(task), queue);
        }

        template <typename R>
        auto then_task(Task<R> task, TaskQueue *queue)
        {
            auto fut = task.get_future();
            auto handle = task.assign(queue);
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

    private:
        std::shared_ptr<TaskPromiseSharedState<T>> mState;
    };

    template <>
    struct TaskFuture<void> {

        TaskFuture() = default;

        TaskFuture(std::shared_ptr<TaskPromiseSharedState<void>> state)
            : mState(std::move(state))
        {
        }

        static TaskFuture make_ready()
        {
            return { std::make_shared<TaskPromiseSharedState<void>>(true) };
        }

        bool valid() const
        {
            return static_cast<bool>(mState);
        }

        bool attached() const
        {
            return mState && mState->mAttached;
        }

        void reset()
        {
            mState.reset();
        }

        std::shared_ptr<TaskPromiseSharedState<void>> release()
        {
            return std::move(mState);
        }

        bool is_ready() const
        {
            return mState->is_ready();
        }

        bool await_ready() const
        {
            return is_ready();
        }

        bool await_suspend(TaskHandle handle)
        {
            return mState->then_await(std::move(handle));
        }

        void await_resume() const
        {
            assert(is_ready());
            return mState->get();
        }

        template <typename F>
        auto then(F &&f, TaskQueue *queue)
        {
            auto task = make_task(std::forward<F>(f));

            return then_task(std::move(task), queue);
        }

        template <typename R>
        auto then_task(Task<R> task, TaskQueue *queue)
        {
            auto fut = task.get_future();
            auto handle = task.assign(queue);
            mState->then(std::move(handle));
            return fut;
        }

    private:
        std::shared_ptr<TaskPromiseSharedState<void>> mState;
    };

    template <typename T>
    struct AtomicTaskFuture {

        TaskFuture<T> ensure()
        {
            std::shared_ptr<TaskPromiseSharedState<T>> check = mState.load();
            if (!check) {
                std::shared_ptr<TaskPromiseSharedState<T>> state = std::make_shared<TaskPromiseSharedState<T>>();
                if (mState.compare_exchange_strong(check, state))
                    return state;
            }
            return check;
        }

        TaskFuture<T> load() const
        {
            return mState.load();
        }

        void reset()
        {
            mState.store({});
        }

    private:
        std::atomic<std::shared_ptr<TaskPromiseSharedState<T>>> mState;
    };

}
}