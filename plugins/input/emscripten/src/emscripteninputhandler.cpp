#include "emscripteninputlib.h"

#include "emscripteninputhandler.h"

#include "Modules/math/vector2.h"

#include "input/inputevents.h"

UNIQUECOMPONENT(Engine::Input::EmscriptenInputHandler);

namespace Engine {
namespace Input {

    EmscriptenInputHandler::EmscriptenInputHandler(std::tuple<Window::Window *, App::Application &, InputListener *> args)
        : UniqueComponent(std::get<1>(args), std::get<2>(args))
    {
        emscripten_set_mousemove_callback("#canvas", this, 0, EmscriptenInputHandler::handleMouseEvent);

        emscripten_set_mousedown_callback("#canvas", this, 0, EmscriptenInputHandler::handleMouseEvent);
        emscripten_set_mouseup_callback("#document", this, 0, EmscriptenInputHandler::handleMouseEvent);

        //emscripten_set_keydown_callback("#window", this, 0, EmscriptenInputHandler::handleKey);
        //emscripten_set_keyup_callback("#window", this, 0, EmscriptenInputHandler::handleKey);
    }

    EmscriptenInputHandler::~EmscriptenInputHandler()
    {
    }

    bool EmscriptenInputHandler::frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context)
    {

        return true;
    }

    MouseButton::MouseButton EmscriptenInputHandler::convertMouseButton(unsigned short id)
    {
        switch (id) {
        case 0:
            return MouseButton::LEFT_BUTTON;
        case 1:
            return MouseButton::MIDDLE_BUTTON;
        case 2:
            return MouseButton::RIGHT_BUTTON;
        default:
            throw 0;
        }
    }

    EM_BOOL EmscriptenInputHandler::handleMouseEvent(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData)
    {
        EmscriptenInputHandler *_this = static_cast<EmscriptenInputHandler *>(userData);

        switch (eventType) {
        case EMSCRIPTEN_EVENT_MOUSEMOVE:
            _this->injectPointerMove({ { static_cast<float>(mouseEvent->canvasX), static_cast<float>(mouseEvent->canvasY) },
                { static_cast<float>(mouseEvent->movementX), static_cast<float>(mouseEvent->movementY) } });
            return EM_TRUE;
        case EMSCRIPTEN_EVENT_MOUSEDOWN:
            _this->injectPointerPress({ { static_cast<float>(mouseEvent->canvasX), static_cast<float>(mouseEvent->canvasY) },
                convertMouseButton(mouseEvent->button) });
            return EM_TRUE;
        case EMSCRIPTEN_EVENT_MOUSEUP:
            _this->injectPointerRelease({ { static_cast<float>(mouseEvent->canvasX), static_cast<float>(mouseEvent->canvasY) },
                convertMouseButton(mouseEvent->button) });
            return EM_TRUE;
        }

        return EM_FALSE;
    }
}
}

RegisterType(Engine::Input::EmscriptenInputHandler);