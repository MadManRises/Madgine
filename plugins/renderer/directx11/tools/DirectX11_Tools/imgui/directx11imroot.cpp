#include "../directx11toolslib.h"

#include "directx11imroot.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "imgui/imgui.h"
#include "imgui_impl_dx11.h"

#include "im3d/im3d.h"

#include "fontloader.h"

#include "DirectX11/directx11rendercontext.h"

#include "Madgine/window/mainwindow.h"

VIRTUALUNIQUECOMPONENT(Engine::Tools::DirectX11ImRoot)

METATABLE_BEGIN_BASE(Engine::Tools::DirectX11ImRoot, Engine::Tools::ClientImRoot)
METATABLE_END(Engine::Tools::DirectX11ImRoot)

namespace Engine {
namespace Tools {

    DirectX11ImRoot::DirectX11ImRoot(Window::MainWindow &window)
        : VirtualScope(window)
    {
    }

    DirectX11ImRoot::~DirectX11ImRoot()
    {
    }

    Threading::Task<bool> DirectX11ImRoot::init()
    {
        if (!co_await ClientImRoot::init())
            co_return false;

        ImGui_ImplDX11_Init(Render::GetDevice(), Render::GetDeviceContext());
        //ImGui_ImplDirectX11_CreateDeviceObjects();
        /*if (!(ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable))
                ImGui::GetIO().RenderDrawListsFn = ImGui_ImplDX11_RenderDrawData;*/

        /* ImGuiIO &io = ImGui::GetIO();
        io.BackendRendererName = "DirectX 11";
        io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset; // We can honor the ImDrawCmd::VtxOffset field, allowing for large meshes.
        io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;*/ // We can create multi-viewports on the Renderer side (optional)


        Im3D::GetIO().mFetchFont = [](const char *fontName) {
            Render::FontLoader::HandleType font;
            font.load(fontName);
            font.info()->setPersistent(true);

            if (font.available()) {
                return Im3DFont {
                    (Im3DTextureId)font->mTexture->mTextureHandle,
                    font->mTextureSize,
                    font->mGlyphs.data()
                };
            } else {
                return Im3DFont {};
            }
        };

        co_return true;
    }

    Threading::Task<void> DirectX11ImRoot::finalize()
    {
        ImGui_ImplDX11_Shutdown();

        co_await ClientImRoot::finalize();

        co_return;
    }

    void DirectX11ImRoot::newFrame()
    {
        ImGui_ImplDX11_NewFrame();
    }

    void DirectX11ImRoot::renderDrawList(ImGuiViewport *vp)
    {
        ImGui_ImplDX11_RenderDrawData(vp->DrawData);
    }

}
}