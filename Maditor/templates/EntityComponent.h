#pragma once

#include "%moduleHeader"
#include "Scene/Entity/entitycomponent.h"

namespace %game {
	namespace %module {

		class %guard %name : public Engine::Scene::Entity::EntityComponent<%name> {
		public:
			%name(Engine::Scene::Entity::Entity &entity);
		};

	}
}