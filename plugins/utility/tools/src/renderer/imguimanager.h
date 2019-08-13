#pragma once

#include "gui/windowoverlay.h"
#include "Modules/math/vector2.h"

struct ImGuiDockNode;

namespace Engine {
namespace Tools {

    class ImGuiManager : public Engine::GUI::WindowOverlay {
    public:
        ImGuiManager(Engine::App::Application &app);
        virtual ~ImGuiManager();

        virtual void init() = 0;
        virtual void finalize() = 0;

        virtual void newFrame(float timeSinceLastFrame) = 0;
        virtual void render() override;

        bool injectKeyPress(const Engine::Input::KeyEventArgs &arg) override;
        bool injectKeyRelease(const Engine::Input::KeyEventArgs &arg) override;
        bool injectPointerPress(const Engine::Input::PointerEventArgs &arg) override;
        bool injectPointerRelease(const Engine::Input::PointerEventArgs &arg) override;
        bool injectPointerMove(const Engine::Input::PointerEventArgs &arg) override;

        void calculateAvailableScreenSpace(Window::Window *w, Vector3 &pos, Vector3 &size) override;

        void setMenuHeight(float h);
        void setCentralNode(ImGuiDockNode *node);

    protected:
        Engine::App::Application &mApp;
        GUI::TopLevelWindow &mWindow;

        Vector2 mAreaPos = Vector2::ZERO;
        Vector2 mAreaSize = Vector2::ZERO;
    };

}
}