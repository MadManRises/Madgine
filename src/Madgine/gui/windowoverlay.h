#pragma once

#include "../input/inputlistener.h"

namespace Engine {
	namespace GUI {

		struct WindowOverlay : Input::InputListener {
			virtual void render() {}
		};

	}
}