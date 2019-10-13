#include "Madgine/clientlib.h"

#include "gamehandler.h"

#include "Modules/keyvalue/metatable_impl.h"


UNIQUECOMPONENT(ClickBrick::UI::GameHandler)

RegisterType(ClickBrick::UI::GameHandler)

    METATABLE_BEGIN_BASE(ClickBrick::UI::GameHandler, Engine::UI::GuiHandlerBase)
        METATABLE_END(ClickBrick::UI::GameHandler)

            namespace ClickBrick
{
    namespace UI {

        GameHandler::GameHandler(Engine::UI::UIManager &ui)
            : Engine::UI::GuiHandler<GameHandler>(ui, Engine::UI::GuiHandlerBase::WindowType::ROOT_WINDOW)
        {
        }

        const char *GameHandler::key() const
        {
            return "GameHandler";
        }

    }
}
