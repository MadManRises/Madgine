#pragma once

#include "Madgine/ui/guihandler.h"

#include "Madgine/ui/widgetptr.h"

#include "Modules/threading/slot.h"

namespace ClickBrick {
namespace UI {

    struct MainMenuHandler : Engine::UI::GuiHandler<MainMenuHandler> {

        MainMenuHandler(Engine::UI::UIManager &ui);

        virtual const char *key() const override;

		virtual void setWidget(Engine::GUI::WidgetBase *w) override;

        void startGame();


    private:
        Engine::Threading::Slot<&MainMenuHandler::startGame> mStartGameSlot;

		Engine::UI::WidgetPtr<Engine::GUI::Button> mStartGameButton;

    };

}
}