#include "clickbricklib.h"

#include "mainmenuhandler.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "Madgine/widgets/button.h"

#include "gamehandler.h"

#include "gamemanager.h"

UNIQUECOMPONENT(ClickBrick::UI::MainMenuHandler)



    METATABLE_BEGIN_BASE(ClickBrick::UI::MainMenuHandler, Engine::UI::GuiHandlerBase)
        METATABLE_END(ClickBrick::UI::MainMenuHandler)

            namespace ClickBrick
{
    namespace UI {

        MainMenuHandler::MainMenuHandler(Engine::UI::UIManager &ui)
            : Engine::UI::GuiHandler<MainMenuHandler>(ui, Engine::UI::GuiHandlerBase::WindowType::ROOT_WINDOW)
            , mStartGameSlot(this)
            , mStartGameButton(this, "StartGameButton")
        {
        }

        std::string_view MainMenuHandler::key() const
        {
            return "MainMenuHandler";
        }

        void ClickBrick::UI::MainMenuHandler::setWidget(Engine::Widgets::WidgetBase *w)
        {
            if (widget()) {
                mStartGameSlot.disconnectAll();
            }
            Engine::UI::GuiHandlerBase::setWidget(w);
            if (widget()) {
                mStartGameButton->clickEvent().connect(mStartGameSlot);
            }
        }

        void ClickBrick::UI::MainMenuHandler::startGame()
        {
            getGameHandler<GameManager>().start();
            getGuiHandler<GameHandler>().open();
        }

    }
}