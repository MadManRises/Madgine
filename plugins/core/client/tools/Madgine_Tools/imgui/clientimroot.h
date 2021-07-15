#pragma once

#include "Madgine/window/mainwindowcomponent.h"
#include "Madgine/window/mainwindowcomponentcollector.h"

#include "Madgine/threading/framelistener.h"

#include "renderer/imroot.h"

#include "Madgine/render/renderpass.h"

#include "Meta/math/vector2.h"

#include "Interfaces/filesystem/path.h"

struct ImGuiDockNode;
struct ImGuiViewport;

namespace Engine {
namespace Tools {

    struct MADGINE_CLIENT_TOOLS_EXPORT ClientImRoot : Window::MainWindowVirtualBase<ClientImRoot>,
                                                      Threading::FrameListener,
                                                      Render::RenderPass,
                                                      ImRoot {

        ClientImRoot(Window::MainWindow &window);
        ~ClientImRoot();

        bool init() override;
        void finalize() override;

        virtual void newFrame(float timeSinceLastFrame) = 0;
        virtual void render(Render::RenderTarget *target) override;

        virtual void renderMainDrawList() = 0;

        void addViewportMapping(Render::RenderTarget *target, ImGuiViewport *vp);
        void removeViewportMapping(Render::RenderTarget *target);

        virtual int priority() const override;

        bool frameStarted(std::chrono::microseconds timeSinceLastFrame) override;
        bool frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Threading::ContextMask context) override;

        bool injectKeyPress(const Input::KeyEventArgs &arg) override;
        bool injectKeyRelease(const Input::KeyEventArgs &arg) override;
        bool injectPointerPress(const Input::PointerEventArgs &arg) override;
        bool injectPointerRelease(const Input::PointerEventArgs &arg) override;
        bool injectPointerMove(const Input::PointerEventArgs &arg) override;
        bool injectAxisEvent(const Engine::Input::AxisEventArgs &arg) override;

        void setMenuHeight(float h);
        void setCentralNode(ImGuiDockNode *node);

        virtual std::string_view key() const override;

        Rect2i getChildClientSpace() override;        

    private:
        std::map<Render::RenderTarget *, ImGuiViewport *>
            mViewportMappings;

        Vector2 mAreaPos = Vector2::ZERO;
        Vector2 mAreaSize = Vector2::ZERO;

        Filesystem::Path mImGuiIniFilePath;
    };

}
}

RegisterType(Engine::Tools::ClientImRoot);