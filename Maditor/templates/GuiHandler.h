#pragma once

#include "%moduleHeader"
#include "UI/guihandler.h"

namespace %game {
	namespace %module {

		class %guard %name : public Engine::UI::GuiHandler<%name> {
		public:
			%name();
		};

	}
}