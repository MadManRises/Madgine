#include "clickbricklib.h"

#include "gamehandler.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

UNIQUECOMPONENT(ClickBrick::GameHandler)

METATABLE_BEGIN_BASE(ClickBrick::GameHandler, Engine::Input::GuiHandlerBase)
METATABLE_END(ClickBrick::GameHandler)

namespace ClickBrick {

GameHandler::GameHandler(Engine::Input::UIManager &ui)
    : Engine::Input::GuiHandler<GameHandler>(ui, Engine::Input::GuiHandlerBase::WindowType::ROOT_WINDOW, "Ingame")
{
}

std::string_view GameHandler::key() const
{
    return "GameHandler";
}

}
