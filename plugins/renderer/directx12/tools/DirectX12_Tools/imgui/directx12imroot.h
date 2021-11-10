#pragma once

#include "Madgine_Tools/imgui/clientimroot.h"

namespace Engine {
namespace Tools {

    struct DirectX12ImRoot : VirtualScope<DirectX12ImRoot, Window::MainWindowVirtualImpl<DirectX12ImRoot, ClientImRoot>> {

        DirectX12ImRoot(Window::MainWindow &window);
        ~DirectX12ImRoot();

		virtual bool init() override;
        virtual void finalize() override;

        virtual void newFrame(float timeSinceLastFrame) override;
        virtual void renderDrawList(ImGuiViewport *vp) override;        
    };

}
}

RegisterType(Engine::Tools::DirectX12ImRoot);