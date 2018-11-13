#pragma once

#include "Madgine/gui/guisystem.h"

namespace Engine {
	namespace GUI {

		class OpenGLSystem : public GUISystem
		{

		public:
			OpenGLSystem(App::ClientApplication &app);


			virtual bool init() override;

		};

	}
}