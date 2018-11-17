#include "imguidemo.h"

#include "../imgui/imgui.h"

#include "Interfaces/reflection/classname.h"

namespace Engine {
	namespace Tools {

		ImGuiDemo::ImGuiDemo(ImGuiRoot & root) :
			Tool<ImGuiDemo>(root)			
		{
		}

		void ImGuiDemo::render()
		{
			ImGui::ShowDemoWindow();
		}

		const char * ImGuiDemo::key()
		{
			return "ImGuiDemo";
		}

	}
}

