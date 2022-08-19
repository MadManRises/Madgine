#pragma once

#include "Madgine/input/handler.h"

namespace ClickBrick {

    struct MainMenuHandler : Engine::Input::Handler<MainMenuHandler> {
        SERIALIZABLEUNIT(MainMenuHandler)

        MainMenuHandler(Engine::Input::UIManager &ui);

        virtual std::string_view key() const override;

        virtual void setWidget(Engine::Widgets::WidgetBase *w) override;

        void startGame();
    };

}

REGISTER_TYPE(ClickBrick::MainMenuHandler)