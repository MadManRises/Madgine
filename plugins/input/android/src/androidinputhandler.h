#pragma once

struct ALooper;
struct AInputEvent;

#include "Madgine/input/inputcollector.h"
#include "Modules/uniquecomponent/uniquecomponent.h"
#include "Madgine/input/inputhandler.h"

namespace Engine {
namespace Input {

    struct MADGINE_ANDROIDINPUT_EXPORT AndroidInputHandler : public InputHandlerComponent<AndroidInputHandler> {
        AndroidInputHandler(Window::MainWindow &topLevel, Window::Window *window, InputListener *listener);
        ~AndroidInputHandler();

        virtual bool isKeyDown(Key key) override;

    private:
        bool frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Threading::ContextMask context) override;

        bool handleMotionEvent(const AInputEvent *event);

    private:
    };

}
}

RegisterType(Engine::Input::AndroidInputHandler);
