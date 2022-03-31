#include "clickbricklib.h"

#include "mainmenuhandler.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Madgine/widgets/button.h"

#include "gamehandler.h"

#include "gamemanager.h"

UNIQUECOMPONENT(ClickBrick::MainMenuHandler)

METATABLE_BEGIN_BASE(ClickBrick::MainMenuHandler, Engine::Input::GuiHandlerBase)
METATABLE_END(ClickBrick::MainMenuHandler)

namespace ClickBrick {

MainMenuHandler::MainMenuHandler(Engine::Input::UIManager &ui)
    : Engine::Input::GuiHandler<MainMenuHandler>(ui, Engine::Input::GuiHandlerBase::WindowType::ROOT_WINDOW, "MainMenu")
{
}

std::string_view MainMenuHandler::key() const
{
    return "MainMenuHandler";
}

void MainMenuHandler::setWidget(Engine::Widgets::WidgetBase *w)
{
    Engine::Input::GuiHandlerBase::setWidget(w);
    if (widget()) {
        widget()->getChildRecursive<Engine::Widgets::Button>("StartGameButton")->clickEvent().connect(&MainMenuHandler::startGame, this, &mConStore);
    }
}

void MainMenuHandler::startGame()
{
    getGameHandler<GameManager>().start();
    getGuiHandler<GameHandler>().open();
}

}