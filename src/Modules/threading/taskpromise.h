#pragma once

namespace Engine {
namespace Threading {

    struct TaskPromiseTypeBase;

    struct MODULES_EXPORT TaskPromiseSharedStateBase {
        std::mutex mMutex;
        std::vector<TaskHandle> mThenResumes;
        bool mDestroyed = false;

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
    };

    struct MODULES_EXPORT TaskPromiseTypeBase {
        TaskPromiseTypeBase() = default;
        TaskPromiseTypeBase(const TaskPromiseTypeBase &) = delete;
        TaskPromiseTypeBase &operator=(TaskPromiseTypeBase &&) = default;
        ~TaskPromiseTypeBase()
        {
            if (std::shared_ptr<TaskPromiseSharedStateBase> ptr = mState.lock())
                ptr->notifyDestroyed();
        }

        std::suspend_always initial_suspend() noexcept
        {
            return {};
        }

        std::suspend_never final_suspend() noexcept
        {
            if (std::shared_ptr<TaskPromiseSharedStateBase> ptr = mState.lock())
                ptr->finalize();
            return {};
        }

        void unhandled_exception() { }

        void resume(TaskHandle handle);

        void setQueue(TaskQueue *queue);
        TaskQueue *queue() const;

    protected:
        std::weak_ptr<TaskPromiseSharedStateBase> mState;

    private:
        TaskQueue *mQueue = nullptr;
    };

    template <typename T>
    struct TaskPromise : TaskPromiseTypeBase {

        void return_value(T value) noexcept
        {
            if (std::shared_ptr<TaskPromiseSharedStateBase> ptr = mState.lock())
                static_cast<TaskPromiseSharedState<T> *>(ptr.get())->set_value(std::move(value));
        }

        std::shared_ptr<TaskPromiseSharedState<T>> get_state()
        {
            assert(!mState.owner_before(std::weak_ptr<TaskPromiseSharedStateBase> {}) && !std::weak_ptr<TaskPromiseSharedStateBase> {}.owner_before(mState));
            std::shared_ptr<TaskPromiseSharedState<T>> state = std::make_shared<TaskPromiseSharedState<T>>();
            mState = state;
            return state;
        }
    };

    template <>
    struct TaskPromise<void> : TaskPromiseTypeBase {

        void return_void() noexcept
        {
            if (std::shared_ptr<TaskPromiseSharedStateBase> ptr = mState.lock())
                static_cast<TaskPromiseSharedState<void> *>(ptr.get())->set_value();
        }

        std::shared_ptr<TaskPromiseSharedState<void>> get_state()
        {
            assert(!mState.owner_before(std::weak_ptr<TaskPromiseSharedStateBase> {}) && !std::weak_ptr<TaskPromiseSharedStateBase> {}.owner_before(mState));
            std::shared_ptr<TaskPromiseSharedState<void>> state = std::make_shared<TaskPromiseSharedState<void>>();
            mState = state;
            return state;
        }
    };

}
}