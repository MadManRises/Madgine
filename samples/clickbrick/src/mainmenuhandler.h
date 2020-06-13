#pragma once

#include "Madgine/ui/guihandler.h"

#include "Madgine/ui/widgetptr.h"

#include "Modules/threading/slot.h"

namespace ClickBrick {
namespace UI {

    struct MainMenuHandler : Engine::UI::GuiHandler<MainMenuHandler> {

        MainMenuHandler(Engine::UI::UIManager &ui);

        virtual std::string_view key() const override;

		virtual void setWidget(Engine::Widgets::WidgetBase *w) override;

        void startGame();


    private:
        Engine::Threading::Slot<&MainMenuHandler::startGame> mStartGameSlot;

		Engine::UI::WidgetPtr<Engine::Widgets::Button> mStartGameButton;

    };

}
}

RegisterType(ClickBrick::UI::MainMenuHandler);