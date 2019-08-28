#include "../../toolslib.h"

#include "OpenGL/opengllib.h"

#include "openglimguimanager.h"

#include "../../imgui/imgui.h"
#include "../../imgui/examples/imgui_impl_opengl3.h"

#include "OpenGL/openglrenderwindow.h"
#include "OpenGL/openglrendertexture.h"

#include "Modules/math/vector3.h"

#include "Madgine/app/application.h"
#include "Madgine/gui/guisystem.h"
#include "Madgine/gui/widgets/toplevelwindow.h"

namespace Engine {
namespace Tools {

    std::unique_ptr<ImGuiManager> createOpenGlManager(GUI::TopLevelWindow &window)
    {
        return std::make_unique<OpenGLImGuiManager>(window);
    }

    OpenGLImGuiManager::OpenGLImGuiManager(GUI::TopLevelWindow &window)
        : ImGuiManager(window)
    {
    }

    void OpenGLImGuiManager::init()
    {
        ImGui_ImplOpenGL3_Init();
        ImGui_ImplOpenGL3_CreateDeviceObjects();
        if (!(ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable))
			ImGui::GetIO().RenderDrawListsFn = ImGui_ImplOpenGL3_RenderDrawData;
    }

    void OpenGLImGuiManager::finalize()
    {
        ImGui_ImplOpenGL3_Shutdown();
    }

    void OpenGLImGuiManager::newFrame(float timeSinceLastFrame)
    {
        ImGuiIO &io = ImGui::GetIO();

        io.DeltaTime = timeSinceLastFrame;
        
        Vector3 size = mWindow.getScreenSize();

        io.BackendPlatformUserData = &mWindow;

        io.DisplaySize = ImVec2(size.x / io.DisplayFramebufferScale.x, size.y / io.DisplayFramebufferScale.y);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();
    }

    void OpenGLImGuiManager::render(Render::RenderTarget &target) const
    {
        Render::OpenGLRenderTexture &texture = static_cast<Render::OpenGLRenderTexture &>(target);

		ImGui::Image((void *)(intptr_t)texture.texture().handle(), texture.getSize(), { 0, 1 }, { 1, 0 });
    }

}
}