#pragma once

namespace Engine {
namespace Threading {

    struct MODULES_EXPORT TaskPromiseSharedStateBase {

        TaskPromiseSharedStateBase(bool ready = false);
        ~TaskPromiseSharedStateBase();

        std::mutex mMutex;
        std::vector<TaskHandle> mThenResumes;
        std::exception_ptr mException;

        bool mAttached;
        bool mDestroyed;
        bool mDone;

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
            : TaskPromiseSharedStateBase(true)
            , mValue(std::move(val))
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

}
}