#include "../../toolslib.h"

#include "OpenGL/opengllib.h"

#include "openglimguimanager.h"

#include "../../imgui/imgui.h"
#include "../../imgui/examples/imgui_impl_opengl3.h"

#include "OpenGL/openglrenderwindow.h"

#include "Modules/math/vector3.h"

#include "Madgine/app/application.h"
#include "gui/guisystem.h"
#include "gui/widgets/toplevelwindow.h"

namespace Engine {
	namespace Tools {

		std::unique_ptr<ImGuiManager> createOpenGlManager(App::Application &app) {
			return std::make_unique<OpenGLImGuiManager>(app);
		}

		OpenGLImGuiManager::OpenGLImGuiManager(App::Application & app) :
			ImGuiManager(app)
		{
		}

		void OpenGLImGuiManager::init()
		{
			ImGui_ImplOpenGL3_Init();
			ImGui_ImplOpenGL3_CreateDeviceObjects();
			ImGui::GetIO().RenderDrawListsFn = ImGui_ImplOpenGL3_RenderDrawData;
		}

		void OpenGLImGuiManager::finalize()
		{
			ImGui_ImplOpenGL3_Shutdown();
		}

		void OpenGLImGuiManager::newFrame(float timeSinceLastFrame)
		{
			ImGuiIO& io = ImGui::GetIO();

			io.DeltaTime = timeSinceLastFrame;

			Vector3 size = mApp.getGlobalAPIComponent<GUI::GUISystem>().topLevelWindows().front()->getScreenSize();
			
			io.DisplaySize = ImVec2(size.x / io.DisplayFramebufferScale.x, size.y / io.DisplayFramebufferScale.y);

			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

		}

	}
}