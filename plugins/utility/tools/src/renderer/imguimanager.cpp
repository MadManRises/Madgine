#include "../toolslib.h"

#include "imguimanager.h"

#include "Madgine/app/application.h"
#include "client/gui/guisystem.h"
#include "client/gui/widgets/toplevelwindow.h"
#include "client/input/inputhandler.h"

#include "../imgui/imgui.h"

#include "client/input/inputevents.h"

#include "Interfaces/math/vector3.h"

namespace Engine {
	namespace Tools {
		ImGuiManager::ImGuiManager(Engine::App::Application &app) :
			mApp(app)
		{
			app.getGlobalAPIComponent<GUI::GUISystem>().topLevelWindows().front()->addOverlay(this);
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

		void ImGuiManager::calculateAvailableScreenSpace(Vector3 & pos, Vector3 & size)
		{
			pos.y += mMenuHeight;
			size.y -= mMenuHeight;
		}

		void ImGuiManager::setMenuHeight(float h)
		{
			mMenuHeight = h;
		}

	}
}