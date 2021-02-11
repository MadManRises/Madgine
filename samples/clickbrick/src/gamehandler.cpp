#include "clickbricklib.h"

#include "gamehandler.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

UNIQUECOMPONENT(ClickBrick::UI::GameHandler)

METATABLE_BEGIN_BASE(ClickBrick::UI::GameHandler, Engine::UI::GuiHandlerBase)
METATABLE_END(ClickBrick::UI::GameHandler)

SERIALIZETABLE_INHERIT_BEGIN(ClickBrick::UI::GameHandler, Engine::UI::GuiHandlerBase)
SERIALIZETABLE_END(ClickBrick::UI::GameHandler)

namespace ClickBrick {
namespace UI {

    GameHandler::GameHandler(Engine::UI::UIManager &ui)
        : Engine::UI::GuiHandler<GameHandler>(ui, Engine::UI::GuiHandlerBase::WindowType::ROOT_WINDOW)
    {
    }

    std::string_view GameHandler::key() const
    {
        return "GameHandler";
    }

}
}
