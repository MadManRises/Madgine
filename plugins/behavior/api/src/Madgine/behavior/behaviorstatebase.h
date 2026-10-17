#pragma once

#include "Generic/callable_view.h"

#include "Madgine/debug/statedescriptor.h"

namespace Engine {
namespace Behavior {

    struct BehaviorStateBase {
        virtual ~BehaviorStateBase() = default;

        virtual void start() = 0;
        virtual void stop() = 0;
        virtual void destroy()
        {
            delete this;
        }

        Execution::deferred_receiver<BehaviorReceiver> mReceiver;
    };

}
}
