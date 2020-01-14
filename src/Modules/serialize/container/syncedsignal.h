#pragma once

#include "../../threading/signal.h"
#include "action.h"

namespace Engine {
namespace Serialize {

    template <typename Config, typename... _Ty>
    struct SyncedSignal : Threading::Signal<_Ty...> {
        SyncedSignal()
            : mAction(this)
		{

		}
		
		void emit(_Ty... args)
        {
            mAction(std::forward<_Ty>(args)..., {});
        }

    private:
        Action<&Threading::Signal<_Ty...>::emit, Config> mAction;
    };
}
}