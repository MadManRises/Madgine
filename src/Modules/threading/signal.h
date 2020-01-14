#pragma once

#include "signalstub.h"

namespace Engine {
namespace Threading {

    template <typename... _Ty>
    struct Signal : SignalStub<_Ty...> {
        Signal() = default;

        Signal(const Signal<_Ty...> &other) = default;

        Signal(Signal<_Ty...> &&other) noexcept = default;

        ~Signal()
        {
            this->disconnectAll();
        }

        void emit(_Ty... args)
        {
            size_t end = this->mConnectedSlots.size();

            size_t marker = 0;

            for (size_t i = 0; i < end; ++i) {
                if (std::shared_ptr<Connection<_Ty...>> ptr = this->mConnectedSlots[i].lock()) {
                    if (marker < i)
                        this->mConnectedSlots[marker] = ptr;
                    ++marker;
                    (*ptr)(args...);
                }
            }

            this->mConnectedSlots.erase(this->mConnectedSlots.begin() + marker, this->mConnectedSlots.begin() + end);
        }
    };
}
}
