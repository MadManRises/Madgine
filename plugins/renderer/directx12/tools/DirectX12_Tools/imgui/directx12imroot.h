#pragma once

#include "Madgine_Tools/imgui/clientimroot.h"

namespace Engine {
namespace Tools {

    struct DirectX12ImRoot : VirtualScope<DirectX12ImRoot, Window::MainWindowVirtualImpl<DirectX12ImRoot, ClientImRoot>> {

        DirectX12ImRoot(Window::MainWindow &window);
        ~DirectX12ImRoot();

		virtual Threading::Task<bool> init() override;
        virtual Threading::Task<void> finalize() override;

        virtual void newFrame() override;
        virtual void renderDrawList(ImGuiViewport *vp) override;        
    };

}
}

REGISTER_TYPE(Engine::Tools::DirectX12ImRoot)