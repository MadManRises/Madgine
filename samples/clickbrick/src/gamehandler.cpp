#include "clickbricklib.h"

#include "gamehandler.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

UNIQUECOMPONENT(ClickBrick::GameHandler)

METATABLE_BEGIN_BASE(ClickBrick::GameHandler, Engine::Input::HandlerBase)
METATABLE_END(ClickBrick::GameHandler)

namespace ClickBrick {

GameHandler::GameHandler(Engine::Input::UIManager &ui)
    : Engine::Input::Handler<GameHandler>(ui, "Ingame", Engine::Input::HandlerBase::WidgetType::ROOT_WIDGET)
{
}

std::string_view GameHandler::key() const
{
    return "GameHandler";
}

}
