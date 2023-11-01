#pragma once

#include "Generic/execution/algorithm.h"
#include "Generic/execution/execution.h"
#include "Generic/execution/virtualsender.h"
#include "connection.h"
#include "taskqueue.h"

namespace Engine {
namespace Threading {

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

        template <typename T, typename R, typename... Args>
        auto connect(R (T::*f)(Args...), T *t, TaskQueue *queue)
        {
            return connect([t, f](Args... args) { return (t->*f)(std::forward<Args>(args)...); }, queue);
        }

        template <typename T>
        auto connect(T &&slot)
        {
            return sender() | Execution::then(TupleUnpacker::wrap(std::forward<T>(slot))) | Execution::repeat;
        }

        template <typename T>
        auto connect(T &&slot, TaskQueue *queue)
        {
            return sender() | Execution::then([queue, slot { std::forward<T>(slot) }]() mutable { queue->queue(slot); }) | Execution::repeat;
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
