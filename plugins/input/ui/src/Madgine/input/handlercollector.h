#pragma once

#include "Modules/uniquecomponent/uniquecomponentdefine.h"

#include "Meta/keyvalue/virtualscope.h"

DECLARE_UNIQUE_COMPONENT(Engine::Input, Handler, HandlerBase, UIManager &)

namespace Engine {
namespace Input {

    template <typename T>
    using Handler = VirtualScope<T, HandlerComponent<T>>;

}
}
