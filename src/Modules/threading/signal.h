#pragma once

#include "signalstub.h"

namespace Engine {
namespace Threading {

    template <typename... _Ty>
    struct Signal : SignalStub<_Ty...> {
        Signal() = default;

        Signal(const Signal<_Ty...> &other) = default;

        Signal(Signal<_Ty...> &&other) noexcept = default;

        void emit(_Ty... args)
        {
            std::lock_guard guard { mMutex };
            Connection<_Ty...> *current = this->mConnectedSlots;
            this->mConnectedSlots = nullptr;
            while (current) {
                current = current->signal(args...);
            }
        }
    };
}
}
