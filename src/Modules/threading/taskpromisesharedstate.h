#pragma once

#include "Generic/execution/flag.h"
#include "taskhandle.h"

namespace Engine {
namespace Threading {

    struct MODULES_EXPORT TaskPromiseSharedStateBase {

        TaskPromiseSharedStateBase(bool ready = false);
        ~TaskPromiseSharedStateBase();

        std::mutex mMutex;
        std::exception_ptr mException;

        bool mAttached;
        bool mDestroyed;
        bool mDone;

        void attach();
        void finalize();

        void notifyDestroyed();

        void setException(std::exception_ptr exception);
        void rethrowIfException();
    };

    template <typename T>
    struct TaskPromiseSharedState : TaskPromiseSharedStateBase {

        TaskPromiseSharedState() = default;
        TaskPromiseSharedState(T val)
            : TaskPromiseSharedStateBase(true)
            , mFlag(std::move(val))
        {
        }

        Execution::Flag<T> mFlag = std::nullopt;

        bool valid()
        {
            std::lock_guard guard { mMutex };
            return mFlag.isSet() || mException || !mDestroyed;
        }

        bool is_ready()
        {
            std::lock_guard guard { mMutex };
            return mFlag.isSet() || mException;
        }

        void set_value(T val)
        {
            {
                std::lock_guard guard { mMutex };
                mFlag.emplace(std::move(val));
            }
        }

        const T &get()
        {
            std::lock_guard guard { mMutex };
            rethrowIfException();
            assert(mFlag.isSet());
            return std::get<0>(*mFlag);
        }

        auto sender()
        {
            return mFlag.sender();
        }
    };

    template <>
    struct TaskPromiseSharedState<void> : TaskPromiseSharedStateBase {

        TaskPromiseSharedState(bool ready = false)
            : TaskPromiseSharedStateBase(ready)
        {
            if (ready)
                mFlag.emplace();
        }

        Execution::Flag<> mFlag = std::nullopt;

        bool valid()
        {
            std::lock_guard guard { mMutex };
            return mFlag.isSet() || mException || !mDestroyed;
        }

        bool is_ready()
        {
            std::lock_guard guard { mMutex };
            return mFlag.isSet() || mException;
        }

        void set_value()
        {
            {
                std::lock_guard guard { mMutex };
                mFlag.emplace();
            }
        }

        void get()
        {
            std::lock_guard guard { mMutex };
            rethrowIfException();
            assert(mFlag.isSet());
        }

        auto sender()
        {
            return mFlag.sender();
        }
    };

    template <typename T>
    struct TaskFutureAwaitable;

    template <typename T>
    struct TaskFutureAwaitableReceiver : Execution::execution_receiver<> {

        template <typename... V>
        void set_value(V &&...value)
        {
            mState->set_value();
        }

        void set_done()
        {
            throw 0;
        }

        template <typename... E>
        void set_error(E &&...)
        {
            throw 0;
        }

        TaskFutureAwaitable<T> *mState;
    };

    template <typename T>
    struct TaskFutureAwaitable {

        using Sender = std::invoke_result_t<decltype(&TaskPromiseSharedState<T>::sender), TaskPromiseSharedState<T>>;

        using S = Execution::connect_result_t<Sender, TaskFutureAwaitableReceiver<T>>;

        TaskFutureAwaitable(std::shared_ptr<TaskPromiseSharedState<T>> promiseState)
            : mPromiseState(std::move(promiseState))
        {
        }

        bool await_ready()
        {
            return mPromiseState->is_ready();
        }

        bool await_suspend(TaskHandle task)
        {
            mTask = std::move(task);
            construct(mState, DelayedConstruct<S> { [this]() { return Execution::connect(mPromiseState->sender(), TaskFutureAwaitableReceiver<T> { {}, this }); } });
            mState->start();
            
            if (mFlag.test_and_set()) {
                mTask.release();
                return false;
            } else {
                return true;
            }
        }

        decltype(auto) await_resume()
        {
            return mPromiseState->get();
        }

        void set_value()
        {
            destruct(mState);
            if (mFlag.test_and_set())
                mTask.resumeInQueue();            
        }

    private:
        ManualLifetime<S> mState = std::nullopt;
        std::atomic_flag mFlag;
        TaskHandle mTask;
        std::shared_ptr<TaskPromiseSharedState<T>> mPromiseState;
    };

}
}