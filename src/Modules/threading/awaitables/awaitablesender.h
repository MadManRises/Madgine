#pragma once

#include "Generic/execution/sender.h"
#include "Generic/withresult.h"
#include "Generic/makeowning.h"

namespace Engine {
namespace Threading {

    template <typename R, typename V>
    struct AwaitableSenderReceiver {

        void set_value(V value)
        {
            mResult = { std::forward<V>(value) };
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
        WithResult<R, MakeOwning_t<V>> mResult;
    };

    template <typename F, typename R, typename V>
    struct AwaitableSender {

        using S = std::invoke_result_t<Execution::Sender<F, R, V>, Engine::Threading::AwaitableSenderReceiver<R, V> &>;

        AwaitableSender(Execution::Sender<F, R, V> sender)
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

        WithResult<R, MakeOwning_t<V>> await_resume()
        {
            return std::move(mReceiver.mResult);
        }

    private:
        AwaitableSenderReceiver<R, V> mReceiver;
        S mState;
    };

}
}

template <typename F, typename R, typename... V>
auto operator co_await(Engine::Execution::Sender<F, R, V...> sender)
{
    return Engine::Threading::AwaitableSender<F, R, V...> { std::move(sender) };
}
