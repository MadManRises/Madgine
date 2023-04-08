#include "../directx12toolslib.h"

#include "directx12imroot.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "imgui/imgui.h"
#include "imgui_impl_dx12.h"

#include "im3d/im3d.h"

#include "DirectX12/directx12rendercontext.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Madgine/window/mainwindow.h"

#include "DirectX12/directx12rendertarget.h"

UNIQUECOMPONENT(Engine::Tools::DirectX12ImRoot)

METATABLE_BEGIN_BASE(Engine::Tools::DirectX12ImRoot, Engine::Tools::ClientImRoot)
METATABLE_END(Engine::Tools::DirectX12ImRoot)

namespace Engine {
namespace Tools {

    DirectX12ImRoot::DirectX12ImRoot(Window::MainWindow &window)
        : VirtualScope(window)
    {
    }

    DirectX12ImRoot::~DirectX12ImRoot()
    {
    }

    Threading::Task<bool> DirectX12ImRoot::init()
    {
        if (!co_await ClientImRoot::init())
            co_return false;

        OffsetPtr handle = Render::DirectX12RenderContext::getSingleton().mDescriptorHeap.allocate();
        ImGui_ImplDX12_Init(Render::GetDevice(), 2, DXGI_FORMAT_R8G8B8A8_UNORM, Render::DirectX12RenderContext::getSingleton().mDescriptorHeap.resource(), Render::DirectX12RenderContext::getSingleton().mDescriptorHeap.cpuHandle(handle), Render::DirectX12RenderContext::getSingleton().mDescriptorHeap.gpuHandle(handle));
        //ImGui_ImplDirectX12_CreateDeviceObjects();
        /*if (!(ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable))
                ImGui::GetIO().RenderDrawListsFn = [](ImDrawData *data) {
                    ImGui_ImplDX12_RenderDrawData(data, Render::DirectX12RenderContext::getSingleton().mCommandList.mList);
                };*/

        co_return true;
    }

    Threading::Task<void> DirectX12ImRoot::finalize()
    {
        ImGui_ImplDX12_Shutdown();

        co_await ClientImRoot::finalize();

        co_return;
    }

    void DirectX12ImRoot::newFrame()
    {       

        ImGui_ImplDX12_NewFrame();
    }

    void DirectX12ImRoot::renderViewport(Render::RenderTarget *target, ImGuiViewport *vp)
    {
        ImGui_ImplDX12_RenderDrawData(vp->DrawData, static_cast<Render::DirectX12RenderTarget *>(target)->mCommandList);
    }

}
}