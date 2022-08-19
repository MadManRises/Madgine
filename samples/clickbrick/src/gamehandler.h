#pragma once

#include "Madgine/input/handler.h"

namespace ClickBrick {

    struct GameHandler : Engine::Input::Handler<GameHandler> {

        GameHandler(Engine::Input::UIManager &ui);        

        virtual std::string_view key() const override;		    

    };

}

REGISTER_TYPE(ClickBrick::GameHandler)