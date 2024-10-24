#include "../opengltoolslib.h"

#include "openglimroot.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "imgui/imgui.h"
#include "imgui_impl_opengl3.h"

#include "im3d/im3d.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Madgine/window/mainwindow.h"

VIRTUALUNIQUECOMPONENT(Engine::Tools::OpenGLImRoot)

METATABLE_BEGIN_BASE(Engine::Tools::OpenGLImRoot, Engine::Tools::ClientImRoot)
METATABLE_END(Engine::Tools::OpenGLImRoot)



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

        Threading::Task<bool> OpenGLImRoot::init()
        {
            if (!co_await ClientImRoot::init())
                co_return false;

            ImGui_ImplOpenGL3_Init();
            ImGui_ImplOpenGL3_CreateDeviceObjects();
            /*if (!(ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable))
                ImGui::GetIO().RenderDrawListsFn = ImGui_ImplOpenGL3_RenderDrawData;*/

			co_return true;
        }

        Threading::Task<void> OpenGLImRoot::finalize()
        {           
			ImGui_ImplOpenGL3_Shutdown();

			co_await ClientImRoot::finalize();

            co_return;
        }

        void OpenGLImRoot::newFrame()
        {
            ImGui_ImplOpenGL3_NewFrame();
        }

        void OpenGLImRoot::renderViewport(ImGuiViewport *vp)
        {
            ImGui_ImplOpenGL3_RenderDrawData(vp->DrawData);
        }

    }
}
