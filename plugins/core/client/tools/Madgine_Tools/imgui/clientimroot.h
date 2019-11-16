#pragma once

#include "Madgine/gui/toplevelwindow.h"
#include "Madgine/threading/framelistener.h"

#include "renderer/imroot.h"

#include "immanager.h"


namespace Engine {
namespace Tools {

    struct MADGINE_CLIENT_TOOLS_EXPORT ClientImRoot : GUI::TopLevelWindowComponent<ClientImRoot>,
                                                      Threading::FrameListener {

        ClientImRoot(GUI::TopLevelWindow &window);

        const ImManager &manager() const;

        bool init() override;
        void finalize() override;

        bool frameStarted(std::chrono::microseconds timeSinceLastFrame) override;
        bool frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Threading::ContextMask context) override;

        bool injectKeyPress(const Input::KeyEventArgs &arg) override;
        bool injectKeyRelease(const Input::KeyEventArgs &arg) override;
        bool injectPointerPress(const Input::PointerEventArgs &arg) override;
        bool injectPointerRelease(const Input::PointerEventArgs &arg) override;
        bool injectPointerMove(const Input::PointerEventArgs &arg) override;

        void setMenuHeight(float h);
        void setCentralNode(ImGuiDockNode *node);

        virtual const char *key() const override;

        ImRoot mRoot;

		Rect2i getChildClientSpace() override;

    private:
        std::optional<ImManagerSelector> mManager;

        Vector2 mAreaPos = Vector2::ZERO;
        Vector2 mAreaSize = Vector2::ZERO;
    };

}
}