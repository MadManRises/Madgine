#pragma once

#include "Generic/coroutines/handle.h"

namespace Engine {
namespace Threading {

    struct TaskPromiseTypeBase;

    struct TaskPromiseSharedStateBase {
        std::mutex mMutex;
        std::vector<CoroutineHandle<TaskPromiseTypeBase>> mThenResumes;
        bool mDestroyed = false;

        TaskQueue *mQueue = nullptr;
        Barrier *mBarrier = nullptr;

        void finalize();

        void notifyDestroyed();

        void then_resume(CoroutineHandle<TaskPromiseTypeBase> handle)
        {
            std::lock_guard guard { mMutex };
            mThenResumes.emplace_back(std::move(handle));
        }
    };

    template <typename T>
    struct TaskPromiseSharedState : TaskPromiseSharedStateBase {

        TaskPromiseSharedState() = default;
        TaskPromiseSharedState(T val)
            : mValue(std::move(val))
        {
        }

        std::optional<T> mValue;

        bool valid() {
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

        bool valid() {
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
    };

    struct MODULES_EXPORT TaskPromiseTypeBase {
        TaskPromiseTypeBase(std::shared_ptr<TaskPromiseSharedStateBase> state)
            : mState(std::move(state))
        {
        }
        TaskPromiseTypeBase(const TaskPromiseTypeBase &) = delete;
        TaskPromiseTypeBase &operator=(TaskPromiseTypeBase &&) = default;
        ~TaskPromiseTypeBase()
        {
            mState->notifyDestroyed();
        }

        std::suspend_always initial_suspend() noexcept
        {
            return {};
        }

        std::suspend_always final_suspend() noexcept
        {
            mState->finalize();
            return {};
        }

        void unhandled_exception() { }

        void resume(CoroutineHandle<TaskPromiseTypeBase> handle);

        void setQueue(TaskQueue *queue, Barrier *barrier = nullptr);

        

        std::shared_ptr<TaskPromiseSharedStateBase> mState;
    };

}
}