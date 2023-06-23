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
        void connect(R (T::*f)(Args...), T *t)
        {
            connect([t, f](Args... args) { return (t->*f)(std::forward<Args>(args)...); });
        }

        template <typename T, typename R, typename... Args>
        void connect(R (T::*f)(Args...), T *t, TaskQueue *queue)
        {
            connect([t, f](Args... args) { return (t->*f)(std::forward<Args>(args)...); }, queue);
        }

        template <typename T>
        void connect(T &&slot)
        {
            Execution::detach(
                sender() | Execution::then(TupleUnpacker::wrap(std::forward<T>(slot))) | Execution::repeat);
        }

        template <typename T>
        void connect(T &&slot, TaskQueue *queue)
        {
            Execution::detach(
                sender() | Execution::then([queue, slot { std::forward<T>(slot) }]() mutable { queue->queue(slot); }) | Execution::repeat);
        }

        auto sender()
        {
            return Execution::make_virtual_sender<Connection<Ty...>, GenericResult, Ty...>(this);
        }

        template <typename T, typename R, typename... Args>
        void connect(R (T::*f)(Args...), T *t, std::stop_token stop)
        {
            connect([t, f](Args... args) { return (t->*f)(std::forward<Args>(args)...); }, stop);
        }

        template <typename T, typename R, typename... Args>
        void connect(R (T::*f)(Args...), T *t, TaskQueue *queue, std::stop_token stop)
        {
            connect([t, f](Args... args) { return (t->*f)(std::forward<Args>(args)...); }, queue, stop);
        }

        template <typename T>
        void connect(T &&slot, std::stop_token stop)
        {
            Execution::detach(
                sender(stop) | Execution::then(TupleUnpacker::wrap(std::forward<T>(slot))) | Execution::repeat);
        }

        template <typename T>
        void connect(T &&slot, TaskQueue *queue, std::stop_token stop)
        {
            Execution::detach(
                sender(stop) | Execution::then([queue, slot { std::forward<T>(slot) }]<typename... Args>(Args&&... args) mutable { queue->queueTask(make_task(TupleUnpacker::wrap(slot), std::forward<Args>(args)...)); }) | Execution::repeat);
        }

        auto sender(std::stop_token stop)
        {
            return Execution::make_virtual_sender<StoppableConnection<Ty...>, GenericResult, Ty...>(this, stop);
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
