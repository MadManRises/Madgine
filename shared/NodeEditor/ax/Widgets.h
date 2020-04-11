#pragma once
#include "../imgui_node_editor.h"
#include "Drawing.h"

namespace ax {
namespace Widgets {

using Drawing::IconType;

IMGUI_NODE_EDITOR_EXPORT void Icon(const ImVec2 &size, IconType type, bool filled, const ImVec4 &color = ImVec4(1, 1, 1, 1), const ImVec4 &innerColor = ImVec4(0, 0, 0, 0));

} // namespace Widgets
} // namespace ax