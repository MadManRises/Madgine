#pragma once

#include "Madgine/ui/guihandler.h"

#include "Madgine/ui/widgetptr.h"

namespace ClickBrick {
namespace UI {

    struct MainMenuHandler : Engine::UI::GuiHandler<MainMenuHandler> {
        SERIALIZABLEUNIT(MainMenuHandler);

        MainMenuHandler(Engine::UI::UIManager &ui);

        virtual std::string_view key() const override;

        virtual void setWidget(Engine::Widgets::WidgetBase *w) override;

        void startGame();

    private:
        Engine::UI::WidgetPtr<Engine::Widgets::Button> mStartGameButton;
    };

}
}

RegisterType(ClickBrick::UI::MainMenuHandler);