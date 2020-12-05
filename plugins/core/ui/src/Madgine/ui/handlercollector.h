#pragma once

#include "Modules/uniquecomponent/uniquecomponentdefine.h"

#include "Modules/serialize/virtualserializableunit.h"

#include "Modules/keyvalueutil/virtualscope.h"

DECLARE_UNIQUE_COMPONENT(Engine::UI, GuiHandler, GuiHandlerBase, UIManager &);
DECLARE_UNIQUE_COMPONENT(Engine::UI, GameHandler, GameHandlerBase, UIManager &);

namespace Engine {
namespace UI {

    template <typename T>
    using GuiHandler = Serialize::VirtualUnit<T, VirtualScope<T, GuiHandlerComponent<T>>>;
    template <typename T>
    using GameHandler = Serialize::VirtualUnit<T, VirtualScope<T, GameHandlerComponent<T>>>;

}
}
