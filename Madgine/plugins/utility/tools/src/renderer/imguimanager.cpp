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
			mListener(app.gui().topLevelWindows().front()->input()->listener()),
			mApp(app)
		{
			app.gui().topLevelWindows().front()->input()->setListener(this);
			ImGui::CreateContext();
		}
		ImGuiManager::~ImGuiManager()
		{
			ImGui::DestroyContext();
			mApp.gui().topLevelWindows().front()->input()->setListener(mListener);
		}

		void ImGuiManager::injectKeyPress(const Engine::Input::KeyEventArgs& arg)
		{
			ImGuiIO &io = ImGui::GetIO();

			io.KeysDown[arg.scancode] = true;

			io.AddInputCharacter(arg.text);

			if (!io.WantCaptureKeyboard)
				mListener->injectKeyPress(arg);
		}

		void ImGuiManager::injectKeyRelease(const Engine::Input::KeyEventArgs& arg)
		{
			ImGuiIO &io = ImGui::GetIO();

			io.KeysDown[arg.scancode] = false;
		}

		void ImGuiManager::injectMousePress(const Engine::Input::MouseEventArgs& arg)
		{
			ImGuiIO &io = ImGui::GetIO();
			io.MouseDown[arg.button - 1] = true;

			if (!io.WantCaptureMouse)
				mListener->injectMousePress(arg);
		}

		void ImGuiManager::injectMouseRelease(const Engine::Input::MouseEventArgs& arg)
		{
			ImGuiIO &io = ImGui::GetIO();
			io.MouseDown[arg.button - 1] = false;

			if (!io.WantCaptureMouse)
				mListener->injectMouseRelease(arg);
		}

		void ImGuiManager::injectMouseMove(const Engine::Input::MouseEventArgs& arg)
		{
			ImGuiIO &io = ImGui::GetIO();

			io.MousePos.x = arg.position.x;
			io.MousePos.y = arg.position.y;

			io.MouseWheel = arg.scrollWheel;

			if (!io.WantCaptureMouse)
				mListener->injectMouseMove(arg);
		}

	}
}