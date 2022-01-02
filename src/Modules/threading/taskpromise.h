#pragma once

#include "taskhandle.h"

#if MODULES_ENABLE_TASK_TRACKING
#    include "../debug/tasktracking/tasktracker.h"
#endif

namespace Engine {
namespace Threading {

    struct MODULES_EXPORT TaskPromiseSharedStateBase {
        std::mutex mMutex;
        std::vector<TaskHandle> mThenResumes;

        bool mAttached = false;
        bool mDestroyed = false;

        void attach();
        void finalize();

        void notifyDestroyed();

        void then(TaskHandle handle);
    };

    template <typename T>
    struct TaskPromiseSharedState : TaskPromiseSharedStateBase {

        TaskPromiseSharedState() = default;
        TaskPromiseSharedState(T val)
            : mValue(std::move(val))
        {
        }

        std::optional<T> mValue;

        bool valid()
        {
            std::lock_guard guard { mMutex };
            return static_cast<bool>(mValue) || !mDestroyed;
        }

        bool is_ready()
        {
            std::lock_guard guard { mMutex };
            return static_cast<bool>(mValue);
        }

        void set_value(T val)
        {
            std::lock_guard guard { mMutex };
            assert(!mValue);
            mValue = std::move(val);
        }

        T &get()
        {
            std::lock_guard guard { mMutex };
            assert(mValue);
            return *mValue;
        }
    };

    template <>
    struct TaskPromiseSharedState<void> : TaskPromiseSharedStateBase {
        bool mHasValue = false;

        bool valid()
        {
            std::lock_guard guard { mMutex };
            return mHasValue || !mDestroyed;
        }

        bool is_ready()
        {
            std::lock_guard guard { mMutex };
            return mHasValue;
        }

        void set_value()
        {
            std::lock_guard guard { mMutex };
            assert(!mHasValue);
            mHasValue = true;
        }

        void get()
        {
            std::lock_guard guard { mMutex };
            assert(mHasValue);
        }
    };

    struct MODULES_EXPORT TaskPromiseTypeBase {
        TaskPromiseTypeBase() = default;
        TaskPromiseTypeBase(const TaskPromiseTypeBase &) = delete;
        TaskPromiseTypeBase &operator=(TaskPromiseTypeBase &&) = default;
        ~TaskPromiseTypeBase()
        {
            if (mState)
                mState->notifyDestroyed();
        }

        std::suspend_always initial_suspend() noexcept
        {
            return {};
        }

        auto final_suspend() noexcept
        {
            struct TaskHandleAwaiter {
                bool await_ready() noexcept { return !mHandle; }
                std::coroutine_handle<> await_suspend(std::coroutine_handle<> self) noexcept
                {
                    assert(mHandle);
                    return mHandle.release();
                }
                void await_resume() noexcept { }

                TaskHandle mHandle;
            };
            if (mState)
                mState->finalize();
            return TaskHandleAwaiter { std::move(mThenReturn) };
        }

        void unhandled_exception() { }

        //void then(TaskHandle handle);
        void then_return(TaskHandle handle)
        {
            assert(!mThenReturn);
            mThenReturn = std::move(handle);
        }

        void setQueue(TaskQueue *queue);
        TaskQueue *queue() const;

    protected:
        std::shared_ptr<TaskPromiseSharedStateBase> mState;

    private:
        TaskQueue *mQueue = nullptr;
        TaskHandle mThenReturn;
    };

    template <typename T>
    struct TaskPromise : TaskPromiseTypeBase {

        void return_value(T value) noexcept
        {
            if (mState)
                static_cast<TaskPromiseSharedState<T> *>(mState.get())->set_value(std::move(value));
        }

        std::shared_ptr<TaskPromiseSharedState<T>> get_state()
        {
            assert(!mState);
            std::shared_ptr<TaskPromiseSharedState<T>> state = std::make_shared<TaskPromiseSharedState<T>>();
            state->attach();
            mState = state;
            return state;
        }

        void set_state(std::shared_ptr<TaskPromiseSharedState<T>> state)
        {
            assert(!mState && state);
            state->attach();
            mState = std::move(state);
        }
    };

    template <>
    struct TaskPromise<void> : TaskPromiseTypeBase {

        void return_void() noexcept
        {
            if (mState)
                static_cast<TaskPromiseSharedState<void> *>(mState.get())->set_value();
        }

        std::shared_ptr<TaskPromiseSharedState<void>> get_state()
        {
            assert(!mState);
            std::shared_ptr<TaskPromiseSharedState<void>> state = std::make_shared<TaskPromiseSharedState<void>>();
            state->attach();
            mState = state;
            return state;
        }

        void set_state(std::shared_ptr<TaskPromiseSharedState<void>> state)
        {
            assert(!mState && state);
            state->attach();
            mState = std::move(state);
        }
    };

}
}