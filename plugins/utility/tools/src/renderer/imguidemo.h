#pragma once

#include "../tool.h"

namespace Engine {
	namespace Tools {

		class ImGuiDemo : public Tool<ImGuiDemo>{
		public:
			ImGuiDemo(ImGuiRoot &root);

			void render() override;

			const char *key() override;
		};

	}
}

RegisterType(Engine::Tools::ImGuiDemo);