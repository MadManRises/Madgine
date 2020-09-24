#pragma once

#include "Modules/uniquecomponent/uniquecomponentdefine.h"

DECLARE_UNIQUE_COMPONENT(Engine::UI, GuiHandler, GuiHandlerBase, UIManager &);
DECLARE_UNIQUE_COMPONENT(Engine::UI, GameHandler, GameHandlerBase, UIManager &);

namespace Engine {
namespace UI {

    template <typename T>
    using GuiHandler = GuiHandlerComponent<T>;
    template <typename T>
    using GameHandler = GameHandlerComponent<T>;

}
}
