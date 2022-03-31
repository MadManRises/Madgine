#pragma once

#include "Modules/uniquecomponent/uniquecomponentdefine.h"

#include "Meta/keyvalue/virtualscope.h"

DECLARE_UNIQUE_COMPONENT(Engine::Input, GuiHandler, GuiHandlerBase, UIManager &);
DECLARE_UNIQUE_COMPONENT(Engine::Input, GameHandler, GameHandlerBase, UIManager &);

namespace Engine {
namespace Input {

    struct GuiHandlerBase;
    template <typename T>
    using GuiHandler = VirtualScope<T, GuiHandlerComponent<T>>;
    struct GameHandlerBase;
    template <typename T>
    using GameHandler = VirtualScope<T, GameHandlerComponent<T>>;

}
}
