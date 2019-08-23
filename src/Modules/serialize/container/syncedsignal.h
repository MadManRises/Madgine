#pragma once

#include "../../signalslot/signal.h"
#include "action.h"

namespace Engine {
namespace Serialize {

    template <typename Config, typename... _Ty>
    struct ObservedSignal : SignalSlot::Signal<_Ty...> {
        ObservedSignal()
            : mAction(this)
		{

		}
		
		void emit(_Ty... args)
        {
            mAction(std::forward<_Ty>(args)..., {});
        }

    private:
        Action<&SignalSlot::Signal<_Ty...>::emit, Config> mAction;
    };
}
}