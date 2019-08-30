#include "../toolslib.h"

#include "profiler.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imguiaddons.h"

#include "Modules/debug/profiler/profiler.h"

#include "Modules/generic/transformIt.h"

#include "Madgine/app/application.h"

#include "../renderer/imguiroot.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"
#include "Modules/reflection/classname.h"

UNIQUECOMPONENT(Engine::Tools::Profiler);

namespace Engine {
namespace Tools {

    static void drawStats(const Debug::Profiler::ProcessStats *stats, std::chrono::nanoseconds overallTime, std::chrono::nanoseconds parentTotalTime)
    {
        std::chrono::nanoseconds totalTime = stats->totalTime();

        bool open = ImGui::SpanningTreeNode(stats, stats->function(), stats->children().empty());

        ImGui::NextColumn();

        ImGui::RightAlignDuration(totalTime);

        ImGui::NextColumn();

        ImGui::RightAlignText("%.2f%%", 100.0f * totalTime.count() / parentTotalTime.count());

        ImGui::NextColumn();

        ImGui::RightAlignText("%.2f%%", 100.0f * totalTime.count() / overallTime.count());

        ImGui::NextColumn();

        if (open) {
            ImGui::TreePush();
            for (const std::pair<Debug::Profiler::ProcessStats *const, Debug::Profiler::ProcessStats::Data> &child : stats->children()) {
                drawStats(child.first, overallTime, totalTime);
            }
            ImGui::TreePop();
        }
    }

    Profiler::Profiler(ImGuiRoot &root)
        : Tool<Profiler>(root)
        , mProfiler(Debug::Profiler::Profiler::getCurrent())
    {
    }

    void Profiler::render()
    {
        if (ImGui::Begin("Profiler", &mVisible)) {

            ImGui::BeginColumns("cols", 4);

            for (const Debug::Profiler::ProfilerThread *thread : mProfiler.getThreadStats()) {
                drawStats(&thread->mStats, thread->mStats.totalTime(), thread->mStats.totalTime());
            }

            ImGui::EndColumns();
        }
        ImGui::End();
    }

    const char *Profiler::key() const
    {
        return "Profiler";
    }

}
}

METATABLE_BEGIN(Engine::Tools::Profiler)
METATABLE_END(Engine::Tools::Profiler)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::Profiler, Engine::Tools::ToolBase)
SERIALIZETABLE_END(Engine::Tools::Profiler)

RegisterType(Engine::Tools::Profiler);