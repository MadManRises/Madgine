#pragma once

#include "imgui/imguiaddons.h"

namespace Engine {
namespace Tools {

	MADGINE_CLIENT_TOOLS_EXPORT void InteractiveCamera(const ImGui::InteractiveViewState &state, Render::Camera &camera);

}
}