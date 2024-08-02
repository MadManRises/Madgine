#pragma once

#include "algorithm.h"
#include "connection.h"
#include "execution.h"
#include "virtualsender.h"
#include "container/stack.h"

namespace Engine {
namespace Execution {

    template <typename... Ty>
    struct SignalStub {
        SignalStub() = default;

        SignalStub(const SignalStub<Ty...> &other)
        {
        }

        SignalStub(SignalStub<Ty...> &&) noexcept
        {
        }

        SignalStub<Ty...> &operator=(const SignalStub<Ty...> &other) = delete;

        template <typename T, typename R, typename... Args>
        auto connect(R (T::*f)(Args...), T *t)
        {
            return connect([t, f](Args... args) { return (t->*f)(std::forward<Args>(args)...); });
        }

        template <typename T>
        auto connect(T &&slot)
        {
            return sender() | Execution::then(TupleUnpacker::wrap(std::forward<T>(slot))) | Execution::repeat;
        }

        ConnectionSender<SignalStub<Ty...>, Ty...> sender()
        {
            return ConnectionSender<SignalStub<Ty...>, Ty...> { this };
        }

        void enqueue(Connection<SignalStub<Ty...>, Ty...> *con)
        {
            mStack.push(con);
        }

        void disconnect(Connection<SignalStub<Ty...>, Ty...> *con)
        {
            mStack.disconnect(con);
        }

    protected:
        ConnectionStack<Connection<SignalStub<Ty...>, Ty...>> mStack;
    };

}
}
