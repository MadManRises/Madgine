#pragma once

#include "handler.h"

#include "Interfaces/input/inputevents.h"

#include "handlercollector.h"

namespace Engine {
namespace Input {
    struct MADGINE_UI_EXPORT GameHandlerBase : Handler {

        GameHandlerBase(UIManager &ui, std::string_view widgetName);

        virtual void updateRender(std::chrono::microseconds timeSinceLastFrame);
        virtual void fixedUpdateRender(std::chrono::microseconds timeStep);
        virtual void updateApp(std::chrono::microseconds timeSinceLastFrame);
    };

} // namespace UI

}

REGISTER_TYPE(Engine::Input::GameHandlerBase)
