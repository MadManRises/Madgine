#pragma once

#include "Scripting\Types\globalapicomponent.h"
#include "%moduleHeader"


namespace %game {
	namespace %module {

		class %guard %name : public Engine::Scripting::GlobalAPIComponent<%name>{
		public:
			%name();
		};

	}
}