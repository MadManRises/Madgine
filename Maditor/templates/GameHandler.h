#pragma once

#include "%moduleHeader"
#include "UI/gamehandler.h"

namespace %game {
	namespace %module {

		class %guard %name : public Engine::UI::GameHandler<%name> {
		public:
			%name();
		};

	}
}