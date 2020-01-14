#pragma once

#include "signal.h"

namespace Engine {
namespace Threading {

	template <typename... Args>
    struct SignalFunctor {
        void operator()(Args... args)
        {
            mSignal.emit(args...);
        }

        Threading::SignalStub<Args...> &signal()
        {
            return mSignal;
        }

    private:
        Threading::Signal<Args...> mSignal;
    };

}
}