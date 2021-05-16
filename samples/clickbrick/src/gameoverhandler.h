#pragma once

#include "Madgine/ui/guihandler.h"

#include "Madgine/ui/widgetptr.h"

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
        Engine::UI::WidgetPtr<Engine::Widgets::Label> mScoreLabel;
    };

}
}

RegisterType(ClickBrick::UI::GameOverHandler);