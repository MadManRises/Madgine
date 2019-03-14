#include "../toolslib.h"

#include "imguimanager.h"

#include "Madgine/app/application.h"
#include "client/gui/guisystem.h"
#include "client/gui/widgets/toplevelwindow.h"
#include "client/input/inputhandler.h"

#include "../imgui/imgui.h"

#include "client/input/inputevents.h"

#include "Interfaces/math/vector3.h"

#include "Interfaces/debug/profiler/profiler.h"

namespace Engine {
	namespace Tools {

		ImGuiManager::ImGuiManager(Engine::App::Application &app) :
			mApp(app)
		{
			app.getGlobalAPIComponent<GUI::GUISystem>().topLevelWindows().front()->addOverlay(this);
			ImGui::CreateContext();

#if ANDROID
			ImGui::GetIO().DisplayFramebufferScale = ImVec2(3.0f, 3.0f);
#endif
		}

		ImGuiManager::~ImGuiManager()
		{
			ImGui::DestroyContext();			
		}

		void ImGuiManager::render()
		{
			PROFILE();
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

		bool ImGuiManager::injectPointerPress(const Engine::Input::PointerEventArgs& arg)
		{
			ImGuiIO &io = ImGui::GetIO();
			io.MouseDown[arg.button - 1] = true;

			return io.WantCaptureMouse;				
		}

		bool ImGuiManager::injectPointerRelease(const Engine::Input::PointerEventArgs& arg)
		{
			ImGuiIO &io = ImGui::GetIO();
			io.MouseDown[arg.button - 1] = false;

			return io.WantCaptureMouse;
		}

		bool ImGuiManager::injectPointerMove(const Engine::Input::PointerEventArgs& arg)
		{
			ImGuiIO &io = ImGui::GetIO();

			io.MousePos.x = arg.position.x / io.DisplayFramebufferScale.x;
			io.MousePos.y = arg.position.y / io.DisplayFramebufferScale.y;

			io.MouseWheel = arg.scrollWheel;

			return io.WantCaptureMouse;
		}

		void ImGuiManager::calculateAvailableScreenSpace(Vector3 & pos, Vector3 & size)
		{
			ImGuiIO &io = ImGui::GetIO();

			pos.y += mMenuHeight * io.DisplayFramebufferScale.y;
			size.y -= mMenuHeight * io.DisplayFramebufferScale.y;
		}

		void ImGuiManager::setMenuHeight(float h)
		{
			mMenuHeight = h;
		}

	}
}