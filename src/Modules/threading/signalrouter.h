#pragma once

#include "signal.h"

namespace Engine {
namespace Threading {
    template <typename... _Ty>
    struct SignalRouter : Signal<_Ty...> {
        SignalRouter() = default;

        SignalRouter(const SignalRouter<_Ty...> &other) = delete;

        SignalRouter(SignalRouter<_Ty...> &&) = default;

        ~SignalRouter() = default;

        SignalRouter<_Ty...> &operator=(const SignalRouter<_Ty...> &other)
        {
            /*disconnectAll();
				copyConnections(other);*/
            return *this;
        }

        void operator()(_Ty... args)
        {
            emit(args...);
        }

        ConnectionStore &connectionStore()
        {
            return mConnections;
        }

    private:
        ConnectionStore mConnections;
    };
}
}
