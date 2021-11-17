#pragma once

#include "taskhandle.h"

namespace Engine {
namespace Threading {

    struct TaskPromiseTypeBase;

    struct MODULES_EXPORT TaskPromiseSharedStateBase {
        std::mutex mMutex;
        std::vector<TaskHandle> mThenResumes;
        bool mDestroyed = false;

        void finalize();

        void notifyDestroyed();

        void then_resume(TaskHandle handle)
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

        std::suspend_never final_suspend() noexcept
        {
            mState->finalize();
            return {};
        }

        void unhandled_exception() { }

        void resume(TaskHandle handle);

        TaskQueue *queue() const;

    protected:
        std::shared_ptr<TaskPromiseSharedStateBase> mState;

    public:
        TaskQueue *mQueue = nullptr;
    };

    template <typename T>
    struct TaskPromise : TaskPromiseTypeBase {
        TaskPromise()
            : TaskPromiseTypeBase(std::make_shared<TaskPromiseSharedState<T>>())
        {
        }

        void return_value(T value) noexcept
        {
            static_cast<TaskPromiseSharedState<T> *>(mState.get())->set_value(std::move(value));
        }
    };

    template <>
    struct TaskPromise<void> : TaskPromiseTypeBase {
        TaskPromise()
            : TaskPromiseTypeBase(std::make_shared<TaskPromiseSharedState<void>>())
        {
        }

        void return_void() noexcept
        {
        }
    };

}
}