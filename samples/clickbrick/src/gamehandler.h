#pragma once

#include "Madgine/ui/guihandler.h"

#include "Madgine/ui/widgetptr.h"

namespace ClickBrick {
namespace UI {

    struct GameHandler : Engine::UI::GuiHandler<GameHandler> {

        GameHandler(Engine::UI::UIManager &ui);        

        virtual std::string_view key() const override;		    

    };

}
}

RegisterType(ClickBrick::UI::GameHandler);