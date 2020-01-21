#include "../directx11toolslib.h"

#include "directx11imroot.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "imgui/imgui.h"
#include "imgui_impl_dx11.h"

#include "im3d/im3d.h"

#include "DirectX11/directx11fontloader.h"
#include "DirectX11/directx11fontdata.h"
#include "DirectX11/directx11rendercontext.h"

#include "Madgine/gui/toplevelwindow.h"

VIRTUALUNIQUECOMPONENT(Engine::Tools::DirectX11ImRoot)

METATABLE_BEGIN_BASE(Engine::Tools::DirectX11ImRoot, Engine::Tools::ClientImRoot)
METATABLE_END(Engine::Tools::DirectX11ImRoot)

RegisterType(Engine::Tools::DirectX11ImRoot)

    namespace Engine
{
    namespace Tools {

        DirectX11ImRoot::DirectX11ImRoot(GUI::TopLevelWindow &window)
            : VirtualUniqueComponentImpl(window)
        {
        }

        DirectX11ImRoot::~DirectX11ImRoot()
        {
        }

        bool DirectX11ImRoot::init()
        {
            if (!ClientImRoot::init())
                return false;

            ImGui_ImplDX11_Init(Render::GetDevice(), Render::GetDeviceContext());
            //ImGui_ImplDirectX11_CreateDeviceObjects();
            if (!(ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable))
                ImGui::GetIO().RenderDrawListsFn = ImGui_ImplDX11_RenderDrawData;

            Im3D::GetIO().mFetchFont = [](const char *fontName) {
                Render::DirectX11FontLoader::HandleType font;
				font.load(fontName);

                return Im3DFont {
                    font->mTexture.mTextureHandle,
                    font->mTextureSize,
                    font->mGlyphs.data()
                };
            };

            return true;
        }

        void DirectX11ImRoot::finalize()
        {
            ImGui_ImplDX11_Shutdown();

            ClientImRoot::finalize();
        }

        void DirectX11ImRoot::newFrame(float timeSinceLastFrame)
        {
            ImGuiIO &io = ImGui::GetIO();

            io.DeltaTime = timeSinceLastFrame;

            Vector2i size = mWindow.getScreenSpace().mSize;

            io.BackendPlatformUserData = &mWindow;

            io.DisplaySize = ImVec2(size.x / io.DisplayFramebufferScale.x, size.y / io.DisplayFramebufferScale.y);

            ImGui_ImplDX11_NewFrame();
            ImGui::NewFrame();
            Im3D::NewFrame();
        }

    }
}