#pragma once

struct ALooper;
struct AInputEvent;

#include "Madgine/input/inputcollector.h"
#include <emscripten/html5.h>

namespace Engine {
namespace Input {

    struct MADGINE_EMSCRIPTENINPUT_EXPORT EmscriptenInputHandler : public InputHandlerComponent<EmscriptenInputHandler> {
        EmscriptenInputHandler(GUI::TopLevelWindow &topLevel, Window::Window *window, InputListener *listener);
        ~EmscriptenInputHandler();

        static MouseButton::MouseButton convertMouseButton(unsigned short button);

		virtual bool isKeyDown(Key key) override;

    private:
        bool frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context) override;

        static EM_BOOL handleMouseEvent(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);
        static EM_BOOL handleKeyEvent(int eventType, const EmscriptenKeyboardEvent *keyEvent, void *userData);

		bool mKeyDown[512];
    };

}
}