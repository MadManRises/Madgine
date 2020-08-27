#pragma once

struct ALooper;
struct AInputEvent;

#include "Madgine/input/inputcollector.h"
#include "Madgine/input/inputhandler.h"
#include "Modules/uniquecomponent/uniquecomponent.h"
#include <emscripten/html5.h>

namespace Engine {
namespace Input {

    struct MADGINE_EMSCRIPTENINPUT_EXPORT EmscriptenInputHandler : public InputHandlerComponent<EmscriptenInputHandler> {
        EmscriptenInputHandler(Window::MainWindow &topLevel, Window::OSWindow *window, InputListener *listener);
        ~EmscriptenInputHandler();

        static MouseButton::MouseButton convertMouseButton(unsigned short button);

		virtual bool isKeyDown(Key key) override;

    private:
        bool frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Threading::ContextMask context) override;

        static EM_BOOL handleMouseEvent(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);
        static EM_BOOL handleKeyEvent(int eventType, const EmscriptenKeyboardEvent *keyEvent, void *userData);

    };

}
}

RegisterType(Engine::Input::EmscriptenInputHandler);