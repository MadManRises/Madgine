#pragma once

#include "Madgine_Tools/imgui/clientimroot.h"

namespace Engine {
namespace Tools {

    struct DirectX11ImRoot : VirtualScope<DirectX11ImRoot, Window::MainWindowVirtualImpl<DirectX11ImRoot, ClientImRoot>> {

        DirectX11ImRoot(Window::MainWindow &window);
        ~DirectX11ImRoot();

		virtual Threading::Task<bool> init() override;
        virtual Threading::Task<void> finalize() override;

        virtual void newFrame() override;
        virtual void renderDrawList(ImGuiViewport *vp) override; 
    };

}
}

REGISTER_TYPE(Engine::Tools::DirectX11ImRoot)