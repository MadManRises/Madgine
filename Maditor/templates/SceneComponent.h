#pragma once

#include "%moduleHeader"
#include "Scene\sceneComponent.h"

namespace %game {
	namespace %module {

		class %guard %name : public Engine::Scene::SceneComponent<%name> {
		public:
			%name(Engine::Scene::SceneManagerBase *sceneMgr);
		};

	}
}