#pragma once

#include "Generic/execution/sender.h"
#include "Generic/withresult.h"

namespace Engine {
namespace Threading {

    template <typename V, typename R>
    struct AwaitableSenderReceiver {

        void set_value(R result, V value)
        {
            mResult = { std::forward<R>(result), std::forward<V>(value) };
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

        std::atomic_flag mFlag;
        TaskHandle mTask;
        WithResult<V, R> mResult;
    };

    template <typename S, typename V, typename R>
    struct AwaitableSender {

        template <typename F>
        AwaitableSender(Execution::Sender<F, V, R> sender)
            : mState(sender(mReceiver))
        {
        }

        bool await_ready()
        {
            mState.start();
            return mReceiver.mFlag.test();
        }

        bool await_suspend(TaskHandle task)
        {
            mReceiver.mTask = std::move(task);            
            if (mReceiver.mFlag.test_and_set()) {
                mReceiver.mTask.release();
                return false;
            } else {
                return true;
            }
        }

        WithResult<V, R> await_resume()
        {
            return std::move(mReceiver.mResult);
        }

    private:
        AwaitableSenderReceiver<V, R> mReceiver;
        S mState;
    };

}
}

template <typename F, typename V, typename R>
auto operator co_await(Engine::Execution::Sender<F, V, R> sender)
{
    using S = decltype(sender(std::declval<Engine::Threading::AwaitableSenderReceiver<V, R> &>()));
    return Engine::Threading::AwaitableSender<S, V, R> { std::move(sender) };
}
