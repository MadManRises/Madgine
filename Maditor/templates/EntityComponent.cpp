#include <madginelib.h>
#include "%header"

char const * const %game::%module::%name::sComponentName = "%componentName";
API_IMPL(%game::%module::%name);

namespace %game {
	namespace %module {

		%name::%name(Engine::Scene::Entity::Entity &entity) :
			EntityComponent(entity) {

		}

	}
}