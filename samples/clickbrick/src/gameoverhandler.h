#pragma once

#include "Madgine/ui/guihandler.h"

#include "Madgine/ui/widgetptr.h"

#include "Modules/threading/slot.h"

namespace ClickBrick {
namespace UI {

    struct GameOverHandler : Engine::UI::GuiHandler<GameOverHandler> {

        GameOverHandler(Engine::UI::UIManager &ui);

        virtual std::string_view key() const override;

        virtual void setWidget(Engine::Widgets::WidgetBase *w) override;

        void restartGame();

		void setScore(int score);

    private:
        Engine::Threading::Slot<&GameOverHandler::restartGame> mRestartGameSlot;

        Engine::UI::WidgetPtr<Engine::Widgets::Label> mScoreLabel;
    };

}
}

RegisterType(ClickBrick::UI::GameOverHandler);