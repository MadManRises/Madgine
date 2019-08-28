#pragma once

#include "../imguimanager.h"

namespace Engine {
	namespace Tools {

		class OpenGLImGuiManager : public ImGuiManager {
		public:
			OpenGLImGuiManager(GUI::TopLevelWindow &window);

			virtual void init() override;
			virtual void finalize() override;
			virtual void newFrame(float timeSinceLastFrame) override;

			virtual void render(Render::RenderTarget &target) const override;
		};

	}
}