#pragma once

extern "C" {
#include "../SDL2-2.0.9/include/SDL.h"
}
#include "input/inputcollector.h"

namespace Engine {
namespace Input {

    struct MADGINE_SDLINPUT_EXPORT SDLInputHandler : public InputHandlerComponent<SDLInputHandler> {
        SDLInputHandler(Window::Window *window, App::Application &app, InputListener *listener);
        ~SDLInputHandler();

        MouseButton::MouseButton convertMouseButton(Uint8 id);

		virtual bool isKeyDown(Key key) override;

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
