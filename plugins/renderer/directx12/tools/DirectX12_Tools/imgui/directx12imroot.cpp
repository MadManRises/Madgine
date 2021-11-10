#include "../directx12toolslib.h"

#include "directx12imroot.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "imgui/imgui.h"
#include "imgui_impl_dx12.h"

#include "im3d/im3d.h"

#include "fontloader.h"

#include "DirectX12/directx12rendercontext.h"

#include "Madgine/window/mainwindow.h"

VIRTUALUNIQUECOMPONENT(Engine::Tools::DirectX12ImRoot)

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

    bool DirectX12ImRoot::init()
    {
        if (!ClientImRoot::init())
            return false;

        OffsetPtr handle = Render::DirectX12RenderContext::getSingleton().mDescriptorHeap.allocate();
        ImGui_ImplDX12_Init(Render::GetDevice(), 1, DXGI_FORMAT_R8G8B8A8_UNORM, Render::DirectX12RenderContext::getSingleton().mDescriptorHeap.resource(), Render::DirectX12RenderContext::getSingleton().mDescriptorHeap.cpuHandle(handle), Render::DirectX12RenderContext::getSingleton().mDescriptorHeap.gpuHandle(handle));
        //ImGui_ImplDirectX12_CreateDeviceObjects();
        /*if (!(ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable))
                ImGui::GetIO().RenderDrawListsFn = [](ImDrawData *data) {
                    ImGui_ImplDX12_RenderDrawData(data, Render::DirectX12RenderContext::getSingleton().mCommandList.mList);
                };*/

        Im3D::GetIO().mFetchFont = [](const char *fontName) {
            Render::FontLoader::HandleType font;
            font.load(fontName);

            return Im3DFont {
                (Im3DTextureId)font->mTexture->mTextureHandle,
                font->mTextureSize,
                font->mGlyphs.data()
            };
        };

        return true;
    }

    void DirectX12ImRoot::finalize()
    {
        ImGui_ImplDX12_Shutdown();

        ClientImRoot::finalize();
    }

    void DirectX12ImRoot::newFrame(float timeSinceLastFrame)
    {
        ImGuiIO &io = ImGui::GetIO();

        io.DeltaTime = timeSinceLastFrame;

        Vector2i size = mWindow.getScreenSpace().mSize;

        io.BackendPlatformUserData = &mWindow;

        io.DisplaySize = ImVec2(size.x / io.DisplayFramebufferScale.x, size.y / io.DisplayFramebufferScale.y);

        ImGui_ImplDX12_NewFrame();
        ImGui::NewFrame();
        Im3D::NewFrame();
    }

    void DirectX12ImRoot::renderDrawList(ImGuiViewport *vp)
    {
        ImGui_ImplDX12_RenderDrawData(vp->DrawData, Render::DirectX12RenderContext::getSingleton().mCommandList.mList);
    }

}
}