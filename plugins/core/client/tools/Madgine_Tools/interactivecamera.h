#pragma once

namespace ImGui {
struct InteractiveViewState;
}

namespace Engine {
namespace Tools {

	MADGINE_CLIENT_TOOLS_EXPORT void InteractiveCamera(const ImGui::InteractiveViewState &state, Render::Camera &camera);

}
}