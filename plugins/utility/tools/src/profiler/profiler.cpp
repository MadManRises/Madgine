#include "../toolslib.h"

#include "profiler.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../renderer/imguiaddons.h"

#include "Interfaces/debug/profiler/profiler.h"

#include "Interfaces/generic/transformIt.h"

#include "Madgine/app/application.h"

#include "../renderer/imguiroot.h"

UNIQUECOMPONENT(Engine::Tools::Profiler);

namespace Engine {
	namespace Tools {

		static void drawStats(const Debug::Profiler::ProcessStats *stats, std::chrono::nanoseconds overallTime, std::chrono::nanoseconds parentTotalTime) {
			std::chrono::nanoseconds totalTime = stats->totalTime();

			

			bool open = ImGui::SpanningTreeNode(stats, stats->function(), stats->children().empty());

			ImGui::NextColumn();

			ImGui::RightAlignDuration(totalTime);

			ImGui::NextColumn();

			ImGui::RightAlignText("%.2f%%", 100.0f * totalTime.count() / parentTotalTime.count());

			ImGui::NextColumn();

			ImGui::RightAlignText("%.2f%%", 100.0f * totalTime.count() / overallTime.count());

			ImGui::NextColumn();

			if (open){
				ImGui::TreePush();
				for (const std::pair<Debug::Profiler::ProcessStats *const, Debug::Profiler::ProcessStats::Data> &child : stats->children()) {
					drawStats(child.first, overallTime, totalTime);
				}
				ImGui::TreePop();
			}
		}

		Profiler::Profiler(ImGuiRoot & root) :
			Tool<Profiler>(root),
			mProfiler(Debug::Profiler::Profiler::getCurrent())
		{
		}

		void Profiler::render()
		{
			if (ImGui::Begin("Profiler")) {

				ImGui::BeginColumns("cols", 4);

				for (const Debug::Profiler::ProfilerThread &thread : mProfiler.getThreadStats())
				{
					drawStats(&thread.mStats, thread.mStats.totalTime(), thread.mStats.totalTime());
				}

				ImGui::EndColumns();
			}
			ImGui::End();
		}

		const char * Profiler::key()
		{
			return "Profiler";
		}


	}
}

