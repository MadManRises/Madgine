#pragma once

#include "Modules/uniquecomponent/uniquecomponentdefine.h"

#include "Meta/serialize/hierarchy/virtualserializableunit.h"

#include "Meta/keyvalue/virtualscope.h"

DECLARE_UNIQUE_COMPONENT(Engine::Input, GuiHandler, GuiHandlerBase, UIManager &);
DECLARE_UNIQUE_COMPONENT(Engine::Input, GameHandler, GameHandlerBase, UIManager &);

namespace Engine {
namespace Input {

    struct GuiHandlerBase;
    template <typename T>
    using GuiHandler = Serialize::VirtualData<T, VirtualScope<T, GuiHandlerComponent<T>>>;
    struct GameHandlerBase;
    template <typename T>
    using GameHandler = Serialize::VirtualData<T, VirtualScope<T, GameHandlerComponent<T>>>;

}
}
