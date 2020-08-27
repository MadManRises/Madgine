#pragma once


#include "Madgine/input/inputcollector.h"
#include "Modules/uniquecomponent/uniquecomponent.h"
#include "Madgine/input/inputhandler.h"

namespace Engine {
namespace Input {

    struct MADGINE_DIRECTINPUT_EXPORT DirectInputHandler : public InputHandlerComponent<DirectInputHandler> {
        DirectInputHandler(Window::MainWindow &topLevel, Window::OSWindow *window, InputListener *listener);
        ~DirectInputHandler();

        virtual bool isKeyDown(Key::Key key) override;

    private:
        bool frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Threading::ContextMask context) override;

    private:
    };

}
}

RegisterType(Engine::Input::DirectInputHandler);
