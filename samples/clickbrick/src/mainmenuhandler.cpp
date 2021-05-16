#include "clickbricklib.h"

#include "mainmenuhandler.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Madgine/widgets/button.h"

#include "gamehandler.h"

#include "gamemanager.h"

#include "Meta/serialize/serializetable_impl.h"

UNIQUECOMPONENT(ClickBrick::UI::MainMenuHandler)



    METATABLE_BEGIN_BASE(ClickBrick::UI::MainMenuHandler, Engine::UI::GuiHandlerBase)
        METATABLE_END(ClickBrick::UI::MainMenuHandler)

    SERIALIZETABLE_INHERIT_BEGIN(ClickBrick::UI::MainMenuHandler, Engine::UI::GuiHandlerBase)
        SERIALIZETABLE_END(ClickBrick::UI::MainMenuHandler)

            namespace ClickBrick
{
    namespace UI {

        MainMenuHandler::MainMenuHandler(Engine::UI::UIManager &ui)
            : Engine::UI::GuiHandler<MainMenuHandler>(ui, Engine::UI::GuiHandlerBase::WindowType::ROOT_WINDOW)
            , mStartGameButton(this, "StartGameButton")
        {
        }

        std::string_view MainMenuHandler::key() const
        {
            return "MainMenuHandler";
        }

        void ClickBrick::UI::MainMenuHandler::setWidget(Engine::Widgets::WidgetBase *w)
        {
            Engine::UI::GuiHandlerBase::setWidget(w);
            if (widget()) {
                mStartGameButton->clickEvent().connect(&MainMenuHandler::startGame, this, &mConStore);
            }
        }

        void ClickBrick::UI::MainMenuHandler::startGame()
        {
            getGameHandler<GameManager>().start();
            getGuiHandler<GameHandler>().open();
        }

    }
}