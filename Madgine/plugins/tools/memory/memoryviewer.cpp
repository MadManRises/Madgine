#include "../toolslib.h"

#include "memoryviewer.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

#include "../renderer/imguiroot.h"
#include "Madgine/app/application.h"

#include <experimental/filesystem>

#include "Madgine/scripting/types/scopebase.h"

#include "Madgine/generic/keyvalueiterate.h"

#include "Madgine/core/memory.h"
#include <Windows.h>
#include <DbgHelp.h>

namespace Engine
{
	namespace Tools
	{
		MemoryViewer::MemoryViewer(ImGuiRoot & root) :
			Tool<MemoryViewer>(root),
			mTracker(Core::MemoryTracker::sInstance())
		{
			assert(SymInitialize(GetCurrentProcess(), NULL, true));
		}
		void MemoryViewer::render()
		{
			if (ImGui::Begin("Memory Viewer"))
			{
				ImGui::Value("Raw Memory", static_cast<unsigned int>(mTracker.rawMemory()));
				ImGui::Value("Registered Memory", static_cast<unsigned int>(mTracker.registeredMemory()));
				ImGui::Value("Overhead Memory", static_cast<unsigned int>(mTracker.overhead()));

				for (const auto &trace : mTracker.stacktraces()) {
					
				}
				free(info);

			}
			ImGui::End();
		}
		const char * MemoryViewer::key()
		{
			return "Memory Viewer";
		}
	}
}
