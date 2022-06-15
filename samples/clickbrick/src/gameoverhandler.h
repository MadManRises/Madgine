#pragma once

#include "Madgine/input/guihandler.h"

namespace ClickBrick {

    struct GameOverHandler : Engine::Input::GuiHandler<GameOverHandler> {
        SERIALIZABLEUNIT(GameOverHandler);

        GameOverHandler(Engine::Input::UIManager &ui);

        virtual std::string_view key() const override;

        virtual void setWidget(Engine::Widgets::WidgetBase *w) override;

        void restartGame();

        void setScore(int score);

    private:
        Engine::Widgets::Label *mScoreLabel = nullptr;
    };

}

REGISTER_TYPE(ClickBrick::GameOverHandler)