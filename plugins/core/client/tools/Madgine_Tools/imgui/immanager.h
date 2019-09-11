#pragma once

#include "Madgine/gui/windowoverlay.h"
#include "Modules/math/vector2.h"

#include "Modules/uniquecomponent/uniquecomponentdefine.h"

struct ImGuiDockNode;

namespace Engine {
namespace Tools {

    class MADGINE_TOOLS_EXPORT ImManager : public Engine::GUI::WindowOverlay {
    public:
        ImManager(GUI::TopLevelWindow &window);
        virtual ~ImManager();

        virtual void newFrame(float timeSinceLastFrame) = 0;
        virtual void render() override;
        virtual bool render(Render::RenderTarget &target) const = 0;

        bool injectKeyPress(const Input::KeyEventArgs &arg) override;
        bool injectKeyRelease(const Input::KeyEventArgs &arg) override;
        bool injectPointerPress(const Input::PointerEventArgs &arg) override;
        bool injectPointerRelease(const Input::PointerEventArgs &arg) override;
        bool injectPointerMove(const Input::PointerEventArgs &arg) override;

        void calculateAvailableScreenSpace(Window::Window *w, Vector3 &pos, Vector3 &size) override;

        void setMenuHeight(float h);
        void setCentralNode(ImGuiDockNode *node);

    protected:
        GUI::TopLevelWindow &mWindow;

        Vector2 mAreaPos = Vector2::ZERO;
        Vector2 mAreaSize = Vector2::ZERO;
    };

}
}

DECLARE_UNIQUE_COMPONENT(Engine::Tools, ImManager, MADGINE_CLIENT_TOOLS, ImManager, GUI::TopLevelWindow &);