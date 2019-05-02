#include "../toolslib.h"

#include "imguiaddons.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

namespace ImGui {



	void PushDisabled()
	{
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	void PopDisabled()
	{
		ImGui::PopStyleVar();
		ImGui::PopItemFlag();
	}

	bool SpanningTreeNode(const void *id, const char *label, bool leaf)
	{
		ImGui::PushID(id);
		ImGuiStorage* storage = ImGui::GetStateStorage();
		bool *opened = storage->GetBoolRef(ImGui::GetID(id));
		if (!leaf) {
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			ImGuiContext& g = *GImGui;
			const ImVec2 label_size = ImGui::CalcTextSize("asd", nullptr, false);
			const ImGuiStyle& style = g.Style;
			const float text_base_offset_y = ImMax(0.0f, window->DC.CurrentLineTextBaseOffset); // Latch before ItemSize changes it
			const float frame_height = ImMax(ImMin(window->DC.CurrentLineSize.y, g.FontSize + style.FramePadding.y * 2), label_size.y);
			ImRect frame_bb = ImRect(window->DC.CursorPos, ImVec2(window->Pos.x + ImGui::GetContentRegionMax().x, window->DC.CursorPos.y + frame_height));
			ImGui::RenderArrow(ImVec2(style.FramePadding.x + frame_bb.Min.x, g.FontSize*0.15f + text_base_offset_y + frame_bb.Min.y), *opened ? ImGuiDir_Down : ImGuiDir_Right, 0.70f);			
		}
		ImGui::Indent(20.0f);
		bool b = ImGui::Selectable(label, false, ImGuiSelectableFlags_SpanAllColumns);
		ImGui::Unindent(20.0f);
		if (!leaf && b) {
			*opened = !*opened;
		}
		ImGui::PopID();
		return *opened;
	}

	void Duration(std::chrono::nanoseconds dur)
	{
		if (dur.count() < 1000) {
			ImGui::Text("%lld ns", dur.count());
		}
		else if (dur.count() < 1000000) {
			ImGui::Text("%.3f us", std::chrono::duration_cast<std::chrono::duration<float, std::micro>>(dur).count());
		}
		else if (dur.count() < 1000000000) {
			ImGui::Text("%.4f ms", std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(dur).count());
		}
		else {
			ImGui::Text("%.4f  s", std::chrono::duration_cast<std::chrono::duration<float>>(dur).count());
		}

	}

	void RightAlignDuration(std::chrono::nanoseconds dur)
	{
		if (dur.count() < 1000) {
			ImGui::RightAlignText("%lld ns", dur.count());
		}
		else if (dur.count() < 1000000) {
			ImGui::RightAlignText("%.3f us", std::chrono::duration_cast<std::chrono::duration<float, std::micro>>(dur).count());
		}
		else if (dur.count() < 1000000000) {
			ImGui::RightAlignText("%.4f ms", std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(dur).count());
		}
		else {
			ImGui::RightAlignText("%.4f  s", std::chrono::duration_cast<std::chrono::duration<float>>(dur).count());
		}

	}

	void RightAlignText(const char * s, ...)
	{
		char buffer[1024];
		va_list args;

		va_start(args, s);
		int len = vsprintf(buffer, s, args);
		va_end(args);
		assert(len >= 0);

		ImGuiContext &g = *ImGui::GetCurrentContext();

		ImFont* font = g.Font;
		const float font_size = g.FontSize;

		float w = font->CalcTextSizeA(font_size, FLT_MAX, 0.0f, buffer, buffer + len, NULL).x;

		ImGui::SameLine(ImGui::GetContentRegionAvailWidth() - w);
		ImGui::Text("%s", buffer);
	}

}