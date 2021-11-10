#pragma once

#include "Madgine/ui/guihandler.h"

namespace ClickBrick {
namespace UI {

    struct GameOverHandler : Engine::UI::GuiHandler<GameOverHandler> {
        SERIALIZABLEUNIT(GameOverHandler);

        GameOverHandler(Engine::UI::UIManager &ui);

        virtual std::string_view key() const override;

        virtual void setWidget(Engine::Widgets::WidgetBase *w) override;

        void restartGame();

        void setScore(int score);

    private:
        Engine::Widgets::Label *mScoreLabel = nullptr;
    };

}
}

RegisterType(ClickBrick::UI::GameOverHandler);