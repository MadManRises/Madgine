#pragma once

#include "signal.h"

namespace Engine {
namespace SignalSlot {

	template <typename... Args>
    struct SignalFunctor {
        void operator()(Args... args)
        {
            mSignal.emit(args...);
        }

        SignalSlot::SignalStub<Args...> &signal()
        {
            return mSignal;
        }

    private:
        SignalSlot::Signal<Args...> mSignal;
    };

}
}