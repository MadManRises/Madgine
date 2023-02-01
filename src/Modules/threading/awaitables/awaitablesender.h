#pragma once

#include "Generic/sender.h"

namespace Engine {
namespace Threading {

    template <typename R>
    struct AwaitableSenderReceiver {

        void set_value(R value)
        {
            mResult.emplace(std::forward<R>(value));
            mTask.resumeInQueue();
        }

        TaskHandle mTask;
        std::optional<R> mResult;
    };

    template <typename S, typename R>
    struct AwaitableSender {

        template <typename F>
        AwaitableSender(Engine::Sender<F, R> sender)
            : mState(sender(mReceiver))
        {
        }

        bool await_ready()
        {
            return false;
        }

        void await_suspend(TaskHandle task)
        {
            mReceiver.mTask = std::move(task);
            mState.start();
        }

        R await_resume()
        {
            return std::move(*mReceiver.mResult);
        }

    private:
        AwaitableSenderReceiver<R> mReceiver;
        S mState;
    };

}
}

template <typename F, typename R>
auto operator co_await(Engine::Sender<F, R> sender)
{
    using S = decltype(sender(std::declval<Engine::Threading::AwaitableSenderReceiver<R>&>()));
    return Engine::Threading::AwaitableSender<S, R> { std::move(sender) };
}
