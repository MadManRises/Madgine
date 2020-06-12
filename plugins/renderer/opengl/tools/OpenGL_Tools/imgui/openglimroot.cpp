#include "../opengltoolslib.h"

#include "openglimroot.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "imgui/imgui.h"
#include "imgui_impl_opengl3.h"

#include "im3d/im3d.h"

#include "Madgine/window/mainwindow.h"

#include "fontloader.h"

VIRTUALUNIQUECOMPONENT(Engine::Tools::OpenGLImRoot)

METATABLE_BEGIN_BASE(Engine::Tools::OpenGLImRoot, Engine::Tools::ClientImRoot)
METATABLE_END(Engine::Tools::OpenGLImRoot)

RegisterType(Engine::Tools::OpenGLImRoot)

    namespace Engine
{
    namespace Tools {

        OpenGLImRoot::OpenGLImRoot(Window::MainWindow &window)
            : VirtualScope(window)
        {
        }

        OpenGLImRoot::~OpenGLImRoot()
        {
        }

        bool OpenGLImRoot::init()
        {
            if (!ClientImRoot::init())
                return false;

            ImGui_ImplOpenGL3_Init();
            ImGui_ImplOpenGL3_CreateDeviceObjects();
            if (!(ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable))
                ImGui::GetIO().RenderDrawListsFn = ImGui_ImplOpenGL3_RenderDrawData;

            Im3D::GetIO().mFetchFont = [](const char *fontName) {
                Render::FontLoader::HandleType font;
				font.load(fontName);

                return Im3DFont {
                    font->mTextureHandle,
                    font->mTextureSize,
                    font->mGlyphs.data()
                };
            };

			return true;
        }

        void OpenGLImRoot::finalize()
        {           
			ImGui_ImplOpenGL3_Shutdown();

			ClientImRoot::finalize();
        }

        void OpenGLImRoot::newFrame(float timeSinceLastFrame)
        {
            ImGuiIO &io = ImGui::GetIO();

            io.DeltaTime = timeSinceLastFrame;

            Vector2i size = mWindow.getScreenSpace().mSize;

            io.BackendPlatformUserData = &mWindow;

            io.DisplaySize = ImVec2(size.x / io.DisplayFramebufferScale.x, size.y / io.DisplayFramebufferScale.y);

            ImGui_ImplOpenGL3_NewFrame();
            ImGui::NewFrame();
            Im3D::NewFrame();
        }

    }
}