#include "../../toolslib.h"

#include "OpenGL/opengllib.h"

#include "openglimguimanager.h"

#include "../../imgui/imgui.h"
#include "../../imgui/examples/imgui_impl_opengl3.h"

#include "OpenGL/glad.h"

#include "Interfaces/math/vector3.h"

#include "Madgine/app/clientapplication.h"
#include "Madgine/gui/guisystem.h"
#include "Madgine/gui/widgets/toplevelwindow.h"

namespace Engine {
	namespace Tools {

		std::unique_ptr<ImGuiManager> createOpenGlManager(App::ClientApplication &app) {
			return std::make_unique<OpenGLImGuiManager>(app);
		}

		OpenGLImGuiManager::OpenGLImGuiManager(App::ClientApplication & app) :
			ImGuiManager(app)
		{
		}

		void OpenGLImGuiManager::init()
		{
			ImGui_ImplOpenGL3_Init();
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

			Vector3 size = mApp.gui().topLevelWindows().front()->getScreenSize();
			io.DisplaySize = ImVec2(size.x, size.y);

			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

		}

	}
}