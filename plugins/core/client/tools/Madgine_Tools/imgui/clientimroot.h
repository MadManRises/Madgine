#pragma once

#include "Madgine/window/mainwindowcomponent.h"
#include "Madgine/window/mainwindowcomponentcollector.h"

#include "Madgine_Tools/renderer/imroot.h"

#include "Madgine/render/renderpass.h"

#include "Meta/math/vector2.h"

#include "Interfaces/filesystem/path.h"

#include "Generic/intervalclock.h"

struct ImGuiDockNode;
struct ImGuiViewport;

namespace Engine {
namespace Tools {

    struct MADGINE_CLIENT_TOOLS_EXPORT ClientImRoot : Window::MainWindowVirtualBase<ClientImRoot>,
                                                      ImRoot {

        ClientImRoot(Window::MainWindow &window);
        ~ClientImRoot();

        Threading::Task<bool> init() override;
        Threading::Task<void> finalize() override;

        virtual void newFrame() = 0;
        virtual void render(Render::RenderTarget *target, size_t iteration) override;

        virtual void renderViewport(Render::RenderTarget *target, ImGuiViewport *vp) = 0;

        void addViewportMapping(Render::RenderTarget *target, ImGuiViewport *vp);
        void removeViewportMapping(Render::RenderTarget *target);

        bool injectKeyPress(const Input::KeyEventArgs &arg) override;
        bool injectKeyRelease(const Input::KeyEventArgs &arg) override;
        bool injectPointerPress(const Input::PointerEventArgs &arg) override;
        bool injectPointerRelease(const Input::PointerEventArgs &arg) override;
        bool injectPointerMove(const Input::PointerEventArgs &arg) override;
        bool injectAxisEvent(const Engine::Input::AxisEventArgs &arg) override;
        
        void setCentralNode();

        virtual std::string_view key() const override;

        Rect2i getChildClientSpace() override;

        virtual bool includeInLayout() const override;

        virtual Threading::TaskQueue *taskQueue() const override;

        void addRenderTarget(Render::RenderTarget *target);
        void removeRenderTarget(Render::RenderTarget *target);

        Vector2 mLeftControllerStick, mRightControllerStick;
        int mDPadState = 0;
        float mZAxis = 0;

    private:
        std::map<Render::RenderTarget *, ImGuiViewport *> mViewportMappings;

        Vector2 mAreaPos = Vector2::ZERO;
        Vector2 mAreaSize = Vector2::ZERO;

        Filesystem::Path mImGuiIniFilePath;

        IntervalClock<> mFrameClock;

        std::vector<Render::RenderTarget *> mRenderTargets;
    };

}
}

REGISTER_TYPE(Engine::Tools::ClientImRoot)