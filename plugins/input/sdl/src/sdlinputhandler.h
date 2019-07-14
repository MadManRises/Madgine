#pragma once

extern "C" {
#include "../SDL2-2.0.9/include/SDL.h"
}
#include "input/inputcollector.h"

namespace Engine {
namespace Input {

    struct MADGINE_SDLINPUT_EXPORT SDLInputHandler : public InputHandlerComponent<SDLInputHandler> {
        SDLInputHandler(std::tuple<Window::Window *, App::Application &, InputListener *> args);
        ~SDLInputHandler();

        MouseButton::MouseButton convertMouseButton(Uint8 id);

    private:
        bool
        frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context) override;

        bool keyPressed(const SDL_KeyboardEvent &event);
        bool keyReleased(const SDL_KeyboardEvent &event);
        bool mouseMoved(const SDL_MouseMotionEvent &event);
        bool mousePressed(const SDL_MouseButtonEvent &event);
        bool mouseReleased(const SDL_MouseButtonEvent &event);

    private:
        SDL_Window *mWindow;
    };

}
}
