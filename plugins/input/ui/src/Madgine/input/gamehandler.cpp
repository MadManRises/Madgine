#include "../uilib.h"
#include "gamehandler.h"
#include "Madgine/widgets/widget.h"
#include "uimanager.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Modules/uniquecomponent/uniquecomponentregistry.h"

DEFINE_UNIQUE_COMPONENT(Engine::Input, GameHandler)

METATABLE_BEGIN_BASE(Engine::Input::GameHandlerBase, Engine::Input::Handler)
METATABLE_END(Engine::Input::GameHandlerBase)


namespace Engine {
namespace Input {

    GameHandlerBase::GameHandlerBase(UIManager &ui, std::string_view widgetName)
        : Handler(ui, widgetName)
    {
    }

    void GameHandlerBase::updateRender(std::chrono::microseconds timeSinceLastFrame)
    {
    }

    void GameHandlerBase::fixedUpdateRender(std::chrono::microseconds timeStep)
    {
    }

    void GameHandlerBase::updateApp(std::chrono::microseconds timeSinceLastFrame)
    {
    }

}
} // namespace UI
