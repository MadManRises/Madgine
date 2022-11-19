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
        std::exception_ptr mException;

        bool mAttached = false;
        bool mDestroyed = false;
        bool mDone = false;

        void attach();
        void finalize();

        void notifyDestroyed();

        bool then_await(TaskHandle handle);
        void then(TaskHandle handle);

        void setException(std::exception_ptr exception);
        void rethrowIfException();
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
            return static_cast<bool>(mValue) || mException || !mDestroyed;
        }

        bool is_ready()
        {
            std::lock_guard guard { mMutex };
            return static_cast<bool>(mValue) || mException;
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
            rethrowIfException();
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
            return mHasValue || mException || !mDestroyed;
        }

        bool is_ready()
        {
            std::lock_guard guard { mMutex };
            return mHasValue || mException;
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
            rethrowIfException();
            assert(mHasValue);
        }
    };

    struct TaskHandleFinalSuspend {
        bool await_ready() noexcept { return !mHandle; }
        std::coroutine_handle<> await_suspend(std::coroutine_handle<> self) noexcept
        {
            assert(mHandle);
#if MODULES_ENABLE_TASK_TRACKING
            Debug::Threading::onReturn(self, mHandle.queue());
#endif
            return mHandle.release();
        }
        void await_resume() noexcept { }

        TaskHandle mHandle;
    };

    struct MODULES_EXPORT TaskHandleInitialSuspend {
        bool await_ready() noexcept { return false; }
        template <typename T>
        void await_suspend(std::coroutine_handle<T> self) noexcept
        {
#if ENABLE_TASK_TRACKING
            mPromise = &self.promise();
#endif
        }
        void await_resume() noexcept;

#if ENABLE_TASK_TRACKING
        TaskPromiseTypeBase *mPromise;
#endif
    };

    struct MODULES_EXPORT TaskPromiseTypeBase {
        TaskPromiseTypeBase() = default;
        TaskPromiseTypeBase(const TaskPromiseTypeBase &) = delete;
        TaskPromiseTypeBase(TaskPromiseTypeBase &&) = default;
        TaskPromiseTypeBase &operator=(TaskPromiseTypeBase &&) = default;
        ~TaskPromiseTypeBase()
        {
            if (mState)
                mState->notifyDestroyed();
        }

        TaskHandleInitialSuspend initial_suspend() noexcept
        {
            return {};
        }

        TaskHandleFinalSuspend final_suspend() noexcept
        {
            if (mState)
                mState->finalize();
            return { std::move(mThenReturn) };
        }

        void unhandled_exception()
        {
            if (mState)
                mState->setException(std::current_exception());
            else
                throw;
        }

        void then_return(TaskHandle handle)
        {
            assert(!mThenReturn);
            mThenReturn = std::move(handle);
        }

        void setQueue(TaskQueue *queue);
        TaskQueue *queue() const;

    protected:
        std::shared_ptr<TaskPromiseSharedStateBase> mState;
        TaskHandle mThenReturn;

    private:
        TaskQueue *mQueue = nullptr;
    };

    template <typename T>
    struct TaskPromise : TaskPromiseTypeBase {

        void return_value(T value) noexcept
        {
            if (mState)
                static_cast<TaskPromiseSharedState<T> *>(mState.get())->set_value(std::move(value));
        }

        void set_value(T value)
        {
            return_value(value);
            mState->finalize();
            assert(!mThenReturn);
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

        void set_value() {
            return_void();
            mState->finalize();
            assert(!mThenReturn);
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