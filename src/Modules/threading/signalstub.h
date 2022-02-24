#pragma once

#include "connection.h"
#include "connectionstore.h"

namespace Engine {
namespace Threading {
    extern MODULES_EXPORT std::mutex sSignalConnectMutex;

    template <typename... _Ty>
    struct SignalStub {
    public:
        SignalStub() = default;

        SignalStub(const SignalStub<_Ty...> &other)
        {
        }

        SignalStub(SignalStub<_Ty...> &&) noexcept
        {
        }

        ~SignalStub()
        {
            disconnectAll();
        }

        SignalStub<_Ty...> &operator=(const SignalStub<_Ty...> &other) = delete;

        template <typename T, typename R, typename... Args>
        std::weak_ptr<ConnectionBase> connect(R(T::* f)(Args...), T* t, ConnectionStore* store = nullptr) {
            return connect([t, f](Args... args) { return (t->*f)(std::forward<Args>(args)...); });
        }

        template <typename T>
        std::weak_ptr<ConnectionBase> connect(T&& slot, ConnectionStore* store = nullptr) {
            if (!store)
                store = &ConnectionStore::globalStore();
            std::lock_guard<std::mutex> guard(sSignalConnectMutex);
            std::weak_ptr<Connection<_Ty...>> conn = store->template emplace_front<DirectConnection<T, _Ty...>>(
                std::forward<T>(slot));
            mConnectedSlots.emplace_back(conn);
            return conn;
        }

        template <typename T>
        std::weak_ptr<ConnectionBase> connect(
            T &&slot, TaskQueue *queue, const std::vector<DataMutex *> &dependencies = {}, ConnectionStore * store = nullptr)
        {
            if (!store)
                store = &ConnectionStore::globalStore();
            std::lock_guard<std::mutex> guard(sSignalConnectMutex);
            std::weak_ptr<Connection<_Ty...>> conn = store->template emplace_front<QueuedConnection<T, _Ty...>>(
                std::forward<T>(slot), queue);
            mConnectedSlots.emplace_back(conn);
            return conn;
        }

    protected:
        void disconnectAll()
        {
            for (const std::weak_ptr<Connection<_Ty...>> &conn : mConnectedSlots) {
                if (std::shared_ptr<Connection<_Ty...>> ptr = conn.lock()) {
                    ptr->disconnect();
                }
            }
            mConnectedSlots.clear();
        }

        void copyConnections(const SignalStub<_Ty...> &other)
        {
            for (const std::weak_ptr<Connection<_Ty...>> &conn : other.mConnectedSlots) {
                if (std::shared_ptr<Connection<_Ty...>> ptr = conn.lock()) {
                    mConnectedSlots.emplace_back(ptr->clone());
                }
            }
        }

        std::vector<std::weak_ptr<Connection<_Ty...>>> mConnectedSlots;
    };

}
}
