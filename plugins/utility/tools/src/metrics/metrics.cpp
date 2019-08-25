#include "../toolslib.h"

#include "metrics.h"

#include "../imgui/imgui.h"

#include "../renderer/imguiaddons.h"

#include "Modules/reflection/classname.h"
#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

UNIQUECOMPONENT(Engine::Tools::Metrics);

namespace Engine {
	namespace Tools {

		template <size_t S>
		static void plot(const Debug::History<float, S> &data, const char *label, const char *overlayText = nullptr) {
			if (ImGui::BeginChild(label)) {
				const Debug::HistoryData<float> &d = data.data();
				ImGui::Text("Average: %.5f, Current: %.5f, Min: %.5f, Max: %.5f", d.average(S), data.buffer()[(d.mIndex + S - 1) % S], d.mMin, d.mMax);
				ImGui::PlotHistogram("", data.buffer(), S, static_cast<int>(d.mIndex), overlayText, d.mMin * 1.1f, d.mMax * 1.1f, ImVec2(0, 80));	
			}
			ImGui::EndChild();
		}

		Metrics::Metrics(ImGuiRoot & root) :
			Tool<Metrics>(root),
			mTimeBank(0.0f)
		{
		}

		void Metrics::render()
		{

			if (ImGui::Begin("Metrics", &mVisible)) {
				ImGui::Text("Time/frame: ");
				ImGui::Duration(std::chrono::microseconds(static_cast<long long>(ImGui::GetIO().DeltaTime * 1000000.0f)));
				plot(mFramesTrend, "Frames per Second");
			}
			ImGui::End();
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

		const char * Metrics::key() const
		{
			return "Metrics";
		}

	}
}

METATABLE_BEGIN(Engine::Tools::Metrics)
METATABLE_END(Engine::Tools::Metrics)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::Metrics, Engine::Tools::ToolBase)
SERIALIZETABLE_END(Engine::Tools::Metrics)

RegisterType(Engine::Tools::Metrics);