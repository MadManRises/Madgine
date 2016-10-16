#pragma once

#include "%moduleHeader"
#include "OGRE\sceneComponent.h"

namespace %game {
	namespace %module {

		class %guard %name : public Engine::OGRE::SceneComponent<%name> {
		public:
			%name();
		};

	}
}