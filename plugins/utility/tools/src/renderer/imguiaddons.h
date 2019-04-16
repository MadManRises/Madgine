#pragma once

namespace ImGui {

	void PushDisabled();
	void PopDisabled();

	bool SpanningTreeNode(const void *id, const char *label, bool leaf = false);

	void Duration(std::chrono::nanoseconds dur);
	void RightAlignDuration(std::chrono::nanoseconds dur);

	void RightAlignText(const char *s, ...);

}