#include "emscripteninputlib.h"

#include "emscripteninputhandler.h"

#include "Modules/math/vector2.h"

#include "input/inputevents.h"

#include "Modules/keyvalue/metatable_impl.h"

UNIQUECOMPONENT(Engine::Input::EmscriptenInputHandler);

namespace Engine {
namespace Input {

    EmscriptenInputHandler::EmscriptenInputHandler(Window::Window *window, App::Application &app, InputListener *listener)
        : UniqueComponent(app, listener)
        , mKeyDown {}
    {
        emscripten_set_mousemove_callback("#canvas", this, 0, EmscriptenInputHandler::handleMouseEvent);

        emscripten_set_mousedown_callback("#canvas", this, 0, EmscriptenInputHandler::handleMouseEvent);
        emscripten_set_mouseup_callback("#document", this, 0, EmscriptenInputHandler::handleMouseEvent);

        emscripten_set_keydown_callback("#window", this, 0, EmscriptenInputHandler::handleKeyEvent);
        emscripten_set_keyup_callback("#window", this, 0, EmscriptenInputHandler::handleKeyEvent);
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

    bool EmscriptenInputHandler::isKeyDown(Key key)
    {
        return mKeyDown[key];
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

    EM_BOOL EmscriptenInputHandler::handleKeyEvent(int eventType, const EmscriptenKeyboardEvent *keyEvent, void *userData)
    {
        EmscriptenInputHandler *_this = static_cast<EmscriptenInputHandler *>(userData);

        switch (eventType) {
        case EMSCRIPTEN_EVENT_KEYDOWN:
            _this->mKeyDown[keyEvent->keyCode] = true;
            _this->injectKeyPress({ static_cast<Key>(keyEvent->keyCode), keyEvent->key[0] });
            return EM_TRUE;
        case EMSCRIPTEN_EVENT_KEYUP:
            _this->mKeyDown[keyEvent->keyCode] = false;
            _this->injectKeyRelease({ static_cast<Key>(keyEvent->keyCode) });
            return EM_TRUE;
        }

        return EM_FALSE;
    }
}
}

RegisterType(Engine::Input::EmscriptenInputHandler);

METATABLE_BEGIN(Engine::Input::EmscriptenInputHandler)
METATABLE_END(Engine::Input::EmscriptenInputHandler)