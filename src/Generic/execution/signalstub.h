#pragma once

#include "algorithm.h"
#include "execution.h"
#include "virtualsender.h"
#include "connection.h"

namespace Engine {
namespace Execution {

    template <typename... Ty>
    struct SignalStub {
    public:
        SignalStub() = default;

        SignalStub(const SignalStub<Ty...> &other)
        {
        }

        SignalStub(SignalStub<Ty...> &&) noexcept
        {
        }

        ~SignalStub()
        {
            disconnectAll();
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

        ConnectionSender<Ty...> sender()
        {
            return ConnectionSender<Ty...> { this };
        }

        void enqueue(Connection<Ty...> *con, Connection<Ty...> *&next)
        {
            assert(!next);
            assert(con);
            next = mConnectedSlots;
            mConnectedSlots = con;
        }

        void disconnect(Connection<Ty...> *con)
        {
            std::lock_guard guard { mMutex };
            assert(con);
            if (mConnectedSlots) {
                mConnectedSlots->cancel(con, mConnectedSlots);
            }
        }

    protected:
        void disconnectAll()
        {
            Connection<Ty...> *current;
            {
                std::lock_guard guard { mMutex };
                current = this->mConnectedSlots;
                this->mConnectedSlots = nullptr;
            }
            while (current) {
                current->cancel(current);
            }
        }

        Connection<Ty...> *mConnectedSlots = nullptr;
        std::mutex mMutex;
    };

}
}
