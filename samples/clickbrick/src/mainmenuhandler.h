#pragma once

#include "Madgine/ui/guihandler.h"

namespace ClickBrick {
namespace UI {

    struct MainMenuHandler : Engine::UI::GuiHandler<MainMenuHandler> {
        SERIALIZABLEUNIT(MainMenuHandler);

        MainMenuHandler(Engine::UI::UIManager &ui);

        virtual std::string_view key() const override;

        virtual void setWidget(Engine::Widgets::WidgetBase *w) override;

        void startGame();
    };

}
}

RegisterType(ClickBrick::UI::MainMenuHandler);