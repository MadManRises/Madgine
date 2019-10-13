#pragma once

#include "Madgine/ui/guihandler.h"

#include "Madgine/ui/widgetptr.h"

namespace ClickBrick {
namespace UI {

    struct GameHandler : Engine::UI::GuiHandler<GameHandler> {

        GameHandler(Engine::UI::UIManager &ui);        

        virtual const char *key() const override;		    

    };

}
}