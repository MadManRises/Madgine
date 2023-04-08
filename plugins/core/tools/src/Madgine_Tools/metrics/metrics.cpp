#include "../toolslib.h"

#include "metrics.h"

#include "imgui/imgui.h"
#include "imgui/imguiaddons.h"

#include "Meta/serialize/serializetable_impl.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "../renderer/imroot.h"

UNIQUECOMPONENT(Engine::Tools::Metrics);

namespace Engine {
namespace Tools {

    template <size_t S>
    static void plot(Debug::History<float, S> &data, const char *label, const char *overlayText = nullptr)
    {
        if (ImGui::BeginChild(label)) {
            const Debug::HistoryData<float> &d = data.data();
            ImGui::Text("Average: %.1f, Current: %.1f, Min: %.1f, Max: %.1f", data.average(), data.buffer()[(d.mIndex + S - 1) % S], d.mMin, d.mMax);
            float range = d.mMax - d.mMin;
            ImGui::PlotHistogram("", data.buffer(), S, static_cast<int>(d.mIndex), overlayText, d.mMin - 0.05f * range, d.mMax + 0.05f * range, ImVec2(0, 80));
            if (ImGui::Button("Reset extreme values"))
                data.resetExtremeValues();
        }
        ImGui::EndChild();
    }

    Metrics::Metrics(ImRoot &root)
        : Tool<Metrics>(root)
        , mTimeBank(0.0f)
    {
    }

    void Metrics::render()
    {
        ImGui::SetNextWindowSize({ 500, 125 }, ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Metrics", &mVisible)) {
            ImGui::Text("Time/frame: ");
            ImGui::Duration(std::chrono::microseconds(static_cast<long long>(ImGui::GetIO().DeltaTime * 1000000.0f)));
            plot(mFramesTrend, "Frames per Second");
        }
        ImGui::End();
    }

    void Metrics::renderStatus()
    {
        ImGui::Text("%.2f FPS", mFramesPerSecond.average());
        ImGui::Separator();
    }

    void Metrics::update()
    {
        if (ImGui::GetIO().DeltaTime > 0.0f) {
            float fps = 1.0f / ImGui::GetIO().DeltaTime;
            mFramesPerSecond << fps;
        }

        mTimeBank += ImGui::GetIO().DeltaTime;
        if (mTimeBank >= 0.5f) {
            mTimeBank = fmodf(mTimeBank, 0.5f);
            mFramesTrend << mFramesPerSecond.average();
        }

        ToolBase::update();
    }

    std::string_view Metrics::key() const
    {
        return "Metrics";
    }

}
}

METATABLE_BEGIN_BASE(Engine::Tools::Metrics, Engine::Tools::ToolBase)
METATABLE_END(Engine::Tools::Metrics)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::Metrics, Engine::Tools::ToolBase)
SERIALIZETABLE_END(Engine::Tools::Metrics)

