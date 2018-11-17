#include "../toolslib.h"

#include "imguimanager.h"

#include "Madgine/app/clientapplication.h"
#include "Madgine/gui/guisystem.h"
#include "Madgine/gui/widgets/toplevelwindow.h"
#include "Madgine/input/inputhandler.h"

#include "../imgui/imgui.h"

#include "Madgine/input/inputevents.h"

#include "Madgine/app/clientapplication.h"

namespace Engine {
	namespace Tools {
		ImGuiManager::ImGuiManager(Engine::App::ClientApplication &app) :
			mApp(app)
		{
			app.gui().topLevelWindows().front()->addOverlay(this);
			ImGui::CreateContext();
		}
		ImGuiManager::~ImGuiManager()
		{
			ImGui::DestroyContext();			
		}

		void ImGuiManager::render()
		{
			ImGui::Render();
		}

		bool ImGuiManager::injectKeyPress(const Engine::Input::KeyEventArgs& arg)
		{
			ImGuiIO &io = ImGui::GetIO();

			io.KeysDown[arg.scancode] = true;

			io.AddInputCharacter(arg.text);

			return io.WantCaptureKeyboard;
		}

		bool ImGuiManager::injectKeyRelease(const Engine::Input::KeyEventArgs& arg)
		{
			ImGuiIO &io = ImGui::GetIO();

			io.KeysDown[arg.scancode] = false;

			return io.WantCaptureKeyboard;
		}

		bool ImGuiManager::injectMousePress(const Engine::Input::MouseEventArgs& arg)
		{
			ImGuiIO &io = ImGui::GetIO();
			io.MouseDown[arg.button - 1] = true;

			return io.WantCaptureMouse;				
		}

		bool ImGuiManager::injectMouseRelease(const Engine::Input::MouseEventArgs& arg)
		{
			ImGuiIO &io = ImGui::GetIO();
			io.MouseDown[arg.button - 1] = false;

			return io.WantCaptureMouse;
		}

		bool ImGuiManager::injectMouseMove(const Engine::Input::MouseEventArgs& arg)
		{
			ImGuiIO &io = ImGui::GetIO();

			io.MousePos.x = arg.position.x;
			io.MousePos.y = arg.position.y;

			io.MouseWheel = arg.scrollWheel;

			return io.WantCaptureMouse;
		}

	}
}