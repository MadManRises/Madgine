#include "../vulkantoolslib.h"

#include "vulkanimroot.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "imgui/imgui.h"
#include "imgui_impl_vulkan.h"

#include "im3d/im3d.h"

#include "Vulkan/vulkanrendercontext.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Madgine/window/mainwindow.h"

#include "Vulkan/vulkanrendertarget.h"
#include "Vulkan/vulkanrenderwindow.h"

#include "Madgine/window/toolwindow.h"

UNIQUECOMPONENT(Engine::Tools::VulkanImRoot)

METATABLE_BEGIN_BASE(Engine::Tools::VulkanImRoot, Engine::Tools::ClientImRoot)
METATABLE_END(Engine::Tools::VulkanImRoot)

namespace Engine {
namespace Tools {

    VulkanImRoot::VulkanImRoot(Window::MainWindow &window)
        : VirtualScope(window)
    {
    }

    VulkanImRoot::~VulkanImRoot()
    {
    }

    Threading::Task<bool> VulkanImRoot::init()
    {
        if (!co_await ClientImRoot::init())
            co_return false;

        ImGui::GetPlatformIO().Platform_CreateVkSurface = [](ImGuiViewport *vp, ImU64 vk_inst, const void *vk_allocators, ImU64 *out_vk_surface) {
            *out_vk_surface = reinterpret_cast<uintptr_t>(static_cast<Render::VulkanRenderWindow *>(static_cast<Window::ToolWindow *>(vp->PlatformUserData)->getRenderer())->surface());
            return (int)VK_SUCCESS;
        };

        Render::VulkanRenderContext *context = static_cast<Render::VulkanRenderContext *>(mWindow.getRenderer());
        Render::VulkanRenderWindow *window = static_cast<Render::VulkanRenderWindow *>(mWindow.getRenderWindow());

        ImGui_ImplVulkan_InitInfo init {};
        init.Instance = Render::GetInstance();
        init.PhysicalDevice = Render::GetPhysicalDevice();
        init.Device = Render::GetDevice();
        init.QueueFamily = *context->mIndices.graphicsFamily;
        init.Queue = context->mGraphicsQueue;
        init.DescriptorPool = context->mDescriptorPool;
        init.Subpass = 0;
        init.MinImageCount = window->minImageCount(); // >= 2
        init.ImageCount = window->imageCount(); // >= MinImageCount
        init.MSAASamples = VK_SAMPLE_COUNT_1_BIT; // >= VK_SAMPLE_COUNT_1_BIT (0 -> default to VK_SAMPLE_COUNT_1_BIT)
        init.Allocator = nullptr;
        init.CheckVkResultFn = vkCheck;

        ImGui_ImplVulkan_Init(&init, window->mRenderPass);

        {
            Render::VulkanCommandList list = window->fetchCommandList("upload", true);
            ImGui_ImplVulkan_CreateFontsTexture(list);
        }

        /*if (!(ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable))
                ImGui::GetIO().RenderDrawListsFn = [](ImDrawData *data) {
                    ImGui_ImplDX12_RenderDrawData(data, Render::DirectX12RenderContext::getSingleton().mCommandList.mList);
                };*/

        co_return true;
    }

    Threading::Task<void> VulkanImRoot::finalize()
    {
        ImGui_ImplVulkan_Shutdown();

        co_await ClientImRoot::finalize();

        co_return;
    }

    void VulkanImRoot::newFrame()
    {       

        ImGui_ImplVulkan_NewFrame();
    }

    void VulkanImRoot::renderViewport(Render::RenderTarget *target, ImGuiViewport *vp)
    {
        ImGui_ImplVulkan_RenderDrawData(vp->DrawData, static_cast<Render::VulkanRenderTarget *>(target)->mCommandList);
    }

}
}