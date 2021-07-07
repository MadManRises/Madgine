#pragma once

#include "connectionstore.h"
#include "taskqueue.h"
#include "Generic/linkednode.h"

namespace Engine {
namespace Threading {

    struct MODULES_EXPORT ConnectionBase {
        virtual ~ConnectionBase() = default;
        ConnectionBase() = default;
        ConnectionBase(const ConnectionBase &other) = delete;
        void disconnect();

        DoublyLinkedNode<std::shared_ptr<ConnectionBase>> mNode;

    protected:
        template <typename Con, typename... Args>
        std::weak_ptr<Con> cloneImpl(Args &&... args)
        {
            std::shared_ptr<Con> ptr = ConnectionStore::create<Con>(std::forward<Args>(args)...);
            mNode.link(ptr);
            return ptr;
        }       
        
    };

    template <typename... _Ty>
    struct Connection : ConnectionBase {
        using ConnectionBase::ConnectionBase;

        virtual void operator()(_Ty... args) const = 0;
        virtual std::weak_ptr<Connection<_Ty...>> clone() = 0;
    };

    template <typename T, typename... _Ty>
    struct ConnectionInstance : Connection<_Ty...> {
        ConnectionInstance(T &&impl)
            : mImpl(std::forward<T>(impl))
        {
        }

        ConnectionInstance(const ConnectionInstance &other)
            : mImpl(other.mImpl)
        {
        }

        void operator()(_Ty... args) const
        {
            TupleUnpacker::invoke(mImpl, std::forward<_Ty>(args)...);
        }

    private:
        std::remove_reference_t<T> mImpl;
    };

    template <typename T, typename... _Ty>
    struct ConnectionInstance<T *, _Ty...> : Connection<_Ty...> {    
        ConnectionInstance(T *impl)
            : mImpl(impl)
        {
        }

        ConnectionInstance(const ConnectionInstance &other)
            : mImpl(other.mImpl)
        {
        }

        void operator()(_Ty... args) const override
        {
            TupleUnpacker::invoke(*mImpl, std::forward<_Ty>(args)...);
        }

    private:
        T *mImpl;
    };

    template <typename T, typename... _Ty>
    struct DirectConnection : ConnectionInstance<T, _Ty...> {
        DirectConnection(T &&impl)
            : ConnectionInstance<T, _Ty...>(std::forward<T>(impl))
        {
        }

        DirectConnection(const DirectConnection<T, _Ty...> &other)
            : ConnectionInstance<T, _Ty...>(other)
        {
        }

        std::weak_ptr<Connection<_Ty...>> clone() override
        {
            return this->template cloneImpl<DirectConnection>(*this);
        }

        void operator()(_Ty... args) const override final
        {
            ConnectionInstance<T, _Ty...>::operator()(args...);
        }
    };

    template <typename T, typename... _Ty>
    struct QueuedConnection : ConnectionInstance<T, _Ty...> {    
        QueuedConnection(T &&impl,
            TaskQueue &queue)
            : ConnectionInstance<T, _Ty...>(std::forward<T>(impl))
            , mQueue(queue)
        {
        }

        QueuedConnection(const QueuedConnection<T, _Ty...> &other)
            : ConnectionInstance<T, _Ty...>(other)
            , mQueue(other.mQueue)
        {
        }

        std::weak_ptr<Connection<_Ty...>> clone() override
        {
            return this->template cloneImpl<QueuedConnection>(*this);
        }

        void operator()(_Ty... args) const override final
        {
            std::weak_ptr<ConnectionInstance<T, _Ty...>> ptr = std::static_pointer_cast<ConnectionInstance<T, _Ty...>>(*this->mPrev);
            std::tuple<_Ty...> t = std::make_tuple(args...);
            mQueue.queue([=]() {
                if (std::shared_ptr<ConnectionInstance<T, _Ty...>> innerPtr = ptr.lock()) {
                    TupleUnpacker::invokeExpand([&](_Ty... args) { innerPtr->ConnectionInstance<T, _Ty...>::operator()(args...); }, t);
                }
            });
        }

    private:
        TaskQueue &mQueue;
    };
}
}
