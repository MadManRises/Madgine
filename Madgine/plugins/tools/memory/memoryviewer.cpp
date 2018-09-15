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

				constexpr size_t BUFFER = 512;
				PSYMBOL_INFO info = static_cast<PSYMBOL_INFO>(malloc(sizeof(SYMBOL_INFO) + BUFFER));
				info->MaxNameLen = BUFFER;
				info->SizeOfStruct = sizeof(SYMBOL_INFO);
				for (const auto &trace : mTracker.backtraces()) {
					DWORD64 disp;
					if (SymFromAddr(GetCurrentProcess(), reinterpret_cast<DWORD64>(trace.second.mTrace.front()), &disp, info))
						ImGui::Text(info->Name);
					else
						ImGui::Value("error_", static_cast<unsigned int>(GetLastError()));
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
