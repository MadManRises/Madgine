#pragma once

#include "Generic/execution/algorithm.h"
#include "Generic/execution/sender.h"
#include "Generic/makeowning.h"
#include "Generic/withresult.h"

namespace Engine {
namespace Threading {

    template <typename Sender, typename... V>
    struct AwaitableSenderImpl;

    template <typename Sender>
    using AwaitableSender = typename Sender::template value_types<type_pack>::prepend<Sender>::instantiate<AwaitableSenderImpl>;

    template <typename Sender>
    struct AwaitableReceiver : Execution::execution_receiver<> {

        template <typename... V>
        void set_value(V &&...value)
        {
            mState->set_value(std::forward<V>(value)...);
        }

        void set_done()
        {
            mState->set_done();
        }

        template <typename... R>
        void set_error(R &&...result)
        {
            mState->set_error(std::forward<R>(result)...);
        }

        AwaitableSender<Sender> *mState;
    };

    template <typename Sender, typename... V>
    struct AwaitableSenderImpl;

    template <typename Sender>
    struct AwaitableSenderImpl<Sender> {

        using S = Execution::connect_result_t<Sender, AwaitableReceiver<Sender>>;
        using R = typename Sender::result_type;

        AwaitableSenderImpl(Sender &&sender)
            : mState(Execution::connect(std::forward<Sender>(sender), AwaitableReceiver<Sender> { {}, this }))
        {
        }

        bool await_ready()
        {
            mState.start();
            return mFlag.test();
        }

        bool await_suspend(TaskHandle task)
        {
            mTask = std::move(task);
            if (mFlag.test_and_set()) {
                mTask.release();
                return false;
            } else {
                return true;
            }
        }

        std::optional<R> await_resume()
        {
            return std::move(mResult);
        }

        void set_value()
        {
            mResult.reset();
            if (mFlag.test_and_set())
                mTask.resumeInQueue();
        }

        void set_done()
        {
            if (mFlag.test_and_set())
                mTask.resumeInQueue();
        }

        void set_error(R result)
        {
            mResult = std::forward<R>(result);
            if (mFlag.test_and_set())
                mTask.resumeInQueue();
        }

    private:
        S mState;
        std::atomic_flag mFlag = ATOMIC_FLAG_INIT;
        TaskHandle mTask;
        std::optional<R> mResult;
    };

    template <typename Sender, typename V>
    struct AwaitableSenderImpl<Sender, V> {

        using S = Execution::connect_result_t<Sender, AwaitableReceiver<Sender>>;
        using R = typename Sender::result_type;

        AwaitableSenderImpl(Sender &&sender)
            : mState(Execution::connect(std::forward<Sender>(sender), AwaitableReceiver<Sender> { {}, this }))
        {
        }

        bool await_ready()
        {
            mState.start();
            return mFlag.test();
        }

        bool await_suspend(TaskHandle task)
        {
            mTask = std::move(task);
            if (mFlag.test_and_set()) {
                mTask.release();
                return false;
            } else {
                return true;
            }
        }

        WithResult<R, MakeOwning_t<V>> await_resume()
        {
            return std::move(mResult);
        }

        void set_value(V values)
        {
            mResult = { std::forward<V>(values) };
            if (mFlag.test_and_set())
                mTask.resumeInQueue();
        }

        void set_done()
        {
            if (mFlag.test_and_set())
                mTask.resumeInQueue();
        }

        void set_error(R result)
        {
            mResult = std::forward<R>(result);
            if (mFlag.test_and_set())
                mTask.resumeInQueue();
        }

    private:
        S mState;
        std::atomic_flag mFlag = ATOMIC_FLAG_INIT;
        TaskHandle mTask;
        WithResult<R, MakeOwning_t<V>> mResult;
    };

}
}

template <Engine::Execution::Sender Sender>
auto operator co_await(Sender &&sender)
{
    return Engine::Threading::AwaitableSender<Sender> { std::forward<Sender>(sender) };
}
