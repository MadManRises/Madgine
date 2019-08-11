#include "sdlinputlib.h"

#include "sdlinputhandler.h"

#include "input/inputevents.h"

#include "Interfaces/keyvalue/metatable_impl.h"
#include "Interfaces/reflection/classname.h"

#include "Interfaces/os/osapi.h"

#include "Interfaces/window/windowapi.h"

namespace Engine {
namespace Input {

    SDLInputHandler::SDLInputHandler(Window::Window *window, App::Application &app, InputListener *listener)
        : UniqueComponent(app, listener)
    {
        auto result = SDL_Init(SDL_INIT_VIDEO);
        assert(result == 0);
        mWindow
            = SDL_CreateWindowFrom(reinterpret_cast<const void *>(window->mHandle));
        assert(mWindow);
    }

    SDLInputHandler::~SDLInputHandler()
    {
        //SDL_Quit();
    }

    MouseButton::MouseButton SDLInputHandler::convertMouseButton(Uint8 id)
    {
        switch (id) {
        case SDL_BUTTON_LEFT:
            return MouseButton::LEFT_BUTTON;
        case SDL_BUTTON_RIGHT:
            return MouseButton::RIGHT_BUTTON;
        case SDL_BUTTON_MIDDLE:
            return MouseButton::MIDDLE_BUTTON;
        default:
            throw 0;
		}
    }

    bool SDLInputHandler::frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_MOUSEMOTION:
                return mouseMoved(event.motion);
            case SDL_MOUSEBUTTONDOWN:
                return mousePressed(event.button);
            case SDL_MOUSEBUTTONUP:
                return mouseReleased(event.button);
            case SDL_KEYDOWN:
                return keyPressed(event.key);
            case SDL_KEYUP:
                return keyReleased(event.key);
            }
        }
        return true;
    }

    bool SDLInputHandler::keyPressed(const SDL_KeyboardEvent &event)
    {
        injectKeyPress({ static_cast<Key>(event.keysym.scancode), static_cast<char>(event.keysym.sym) });
        return true;
    }

    bool SDLInputHandler::keyReleased(const SDL_KeyboardEvent &event)
    {
        injectKeyRelease({ static_cast<Key>(event.keysym.scancode), static_cast<char>(event.keysym.sym) });
        return true;
    }

    bool SDLInputHandler::mouseMoved(const SDL_MouseMotionEvent &event)
    {
        injectPointerMove({ { static_cast<float>(event.x), static_cast<float>(event.y) },
            { static_cast<float>(event.xrel), static_cast<float>(event.yrel) } });
        return true;
    }

    bool SDLInputHandler::mousePressed(const SDL_MouseButtonEvent &event)
    {
        injectPointerPress({ { static_cast<float>(event.x), static_cast<float>(event.y) },
            convertMouseButton(event.button) });
        return true;
    }

    bool SDLInputHandler::mouseReleased(const SDL_MouseButtonEvent &event)
    {
        injectPointerRelease({ { static_cast<float>(event.x), static_cast<float>(event.y) },
            convertMouseButton(event.button) });
        return true;
    }

}
}

UNIQUECOMPONENT(Engine::Input::SDLInputHandler);

METATABLE_BEGIN(Engine::Input::SDLInputHandler)
METATABLE_END(Engine::Input::SDLInputHandler)

RegisterType(Engine::Input::SDLInputHandler);
