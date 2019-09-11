#pragma once

#include "../toolscollector.h"

namespace Engine {
	namespace Tools {

		class ImGuiDemo : public Tool<ImGuiDemo>{
		public:
			ImGuiDemo(ImRoot &root);

			void render() override;

			const char *key() const override;
		};

	}
}

