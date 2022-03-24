#pragma once

#include "Madgine/input/guihandler.h"

namespace ClickBrick {

    struct GameHandler : Engine::Input::GuiHandler<GameHandler> {

        GameHandler(Engine::Input::UIManager &ui);        

        virtual std::string_view key() const override;		    

    };

}

RegisterType(ClickBrick::GameHandler);