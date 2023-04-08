#pragma once

#include "Madgine_Tools/imgui/clientimroot.h"

namespace Engine {
namespace Tools {

    struct VulkanImRoot : VirtualScope<VulkanImRoot, Window::MainWindowVirtualImpl<VulkanImRoot, ClientImRoot>> {

        VulkanImRoot(Window::MainWindow &window);
        ~VulkanImRoot();

		virtual Threading::Task<bool> init() override;
        virtual Threading::Task<void> finalize() override;

        virtual void newFrame() override;
        virtual void renderViewport(Render::RenderTarget *target, ImGuiViewport *vp) override;        
    };

}
}

REGISTER_TYPE(Engine::Tools::VulkanImRoot)