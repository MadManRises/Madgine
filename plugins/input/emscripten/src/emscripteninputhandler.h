#pragma once

struct ALooper;
struct AInputEvent;

#include "input/inputcollector.h"
#include <emscripten/html5.h>

namespace Engine {
namespace Input {

    struct MADGINE_EMSCRIPTENINPUT_EXPORT EmscriptenInputHandler : public InputHandlerComponent<EmscriptenInputHandler> {
        EmscriptenInputHandler(std::tuple<Window::Window *, App::Application &, InputListener *> args);
        ~EmscriptenInputHandler();

        static MouseButton::MouseButton convertMouseButton(unsigned short button);

    private:
        bool frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context) override;

        static EM_BOOL handleMouseEvent(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);
    };

}
}