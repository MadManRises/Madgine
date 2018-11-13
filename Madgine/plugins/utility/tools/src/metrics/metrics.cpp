#include "../toolslib.h"

#include "metrics.h"

#include "../imgui/imgui.h"

namespace Engine {
	namespace Tools {

		template <size_t S>
		static void plot(const Debug::History<float, S> &data, const char *label, const char *overlayText = nullptr) {
			if (ImGui::BeginChild(label)) {
				const Debug::HistoryData<float> &d = data.data();
				ImGui::Text("Average: %.5f, Current: %.5f, Min: %.5f, Max: %.5f", d.average(S), data.buffer()[(d.mIndex + S - 1) % S], d.mMin, d.mMax);
				ImGui::PlotHistogram("", data.buffer(), S, d.mIndex, overlayText, d.mMin * 1.1f, d.mMax * 1.1f, ImVec2(0, 80));
				ImGui::EndChild();
			}
		}

		Metrics::Metrics(ImGuiRoot & root) :
			Tool<Metrics>(root),
			mTimeBank(0.0f)
		{
		}

		void Metrics::render()
		{

			if (ImGui::Begin("Metrics")) {
				ImGui::Text("Time/frame: %.5f", ImGui::GetIO().DeltaTime);
				plot(mFramesTrend, "Frames per Second");
				ImGui::End();
			}
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

		const char * Metrics::key()
		{
			return "Metrics";
		}

	}
}