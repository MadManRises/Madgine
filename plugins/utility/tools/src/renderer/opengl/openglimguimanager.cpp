#include "../../toolslib.h"

#include "OpenGL/opengllib.h"

#include "openglimguimanager.h"

#include "../../imgui/imgui.h"
#include "../../imgui/examples/imgui_impl_opengl3.h"

#include "OpenGL/glad.h"
#include "OpenGL/openglcontextguard.h"
#include "OpenGL/openglrenderwindow.h"

#include "Interfaces/math/vector3.h"

#include "Madgine/app/application.h"
#include "client/gui/guisystem.h"
#include "client/gui/widgets/toplevelwindow.h"

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
			Render::OpenGLContextGuard guard = static_cast<Render::OpenGLRenderWindow*>(mApp.getGlobalAPIComponent<GUI::GUISystem>().topLevelWindows().front()->getRenderer())->lockContext();
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
			Render::OpenGLContextGuard guard = static_cast<Render::OpenGLRenderWindow*>(mApp.getGlobalAPIComponent<GUI::GUISystem>().topLevelWindows().front()->getRenderer())->lockContext();
			ImGuiIO& io = ImGui::GetIO();

			io.DeltaTime = timeSinceLastFrame;

			Vector3 size = mApp.getGlobalAPIComponent<GUI::GUISystem>().topLevelWindows().front()->getScreenSize();
			io.DisplaySize = ImVec2(size.x, size.y);

			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

		}

	}
}