#pragma once

#include "Madgine/window/mainwindowcomponent.h"
#include "Madgine/window/mainwindowcomponentcollector.h"

#include "renderer/imroot.h"

#include "Madgine/render/renderpass.h"

#include "Meta/math/vector2.h"

#include "Interfaces/filesystem/path.h"

#include "Generic/intervalclock.h"

struct ImGuiDockNode;
struct ImGuiViewport;

namespace Engine {
namespace Tools {

    struct MADGINE_CLIENT_TOOLS_EXPORT ClientImRoot : Window::MainWindowVirtualBase<ClientImRoot>,
                                                      Render::RenderPass,
                                                      ImRoot {

        ClientImRoot(Window::MainWindow &window);
        ~ClientImRoot();

        Threading::Task<bool> init() override;
        Threading::Task<void> finalize() override;

        virtual void newFrame() = 0;
        virtual void preRender() override;
        virtual void render(Render::RenderTarget *target, size_t iteration) override;

        virtual void renderDrawList(ImGuiViewport *vp) = 0;

        void addViewportMapping(Render::RenderTarget *target, ImGuiViewport *vp);
        void removeViewportMapping(Render::RenderTarget *target);

        virtual int priority() const override;

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

        virtual Threading::TaskQueue *taskQueue() const override;

        void addRenderTarget(Render::RenderTarget *target);
        void removeRenderTarget(Render::RenderTarget *target);

    private:
        std::map<Render::RenderTarget *, ImGuiViewport *>
            mViewportMappings;

        Vector2 mAreaPos = Vector2::ZERO;
        Vector2 mAreaSize = Vector2::ZERO;

        Filesystem::Path mImGuiIniFilePath;

        IntervalClock<std::chrono::steady_clock> mFrameClock;

        std::vector<Render::RenderTarget *> mRenderTargets;
    };

}
}

RegisterType(Engine::Tools::ClientImRoot);