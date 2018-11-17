#pragma once

namespace ImGui {

	void PushDisabled();
	void PopDisabled();

	bool SpanningTreeNode(const void *id, const char *label, bool leaf = false);

	void Duration(std::chrono::nanoseconds dur);

}