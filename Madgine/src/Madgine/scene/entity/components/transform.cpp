#include "../../../baselib.h"

#include "transform.h"

#include "../../../scripting/types/api.h"
#include "../../../generic/keyvalueiterate.h"

RegisterClass(Engine::Scene::Entity::Transform);

namespace Engine
{




	namespace Scene
	{
		namespace Entity
		{
			
			ENTITYCOMPONENTVIRTUALBASE_IMPL(Transform, Transform);

			KeyValueMapList Transform::maps()
			{
				return Scope::maps().merge(MAP(Position, getPosition, setPosition), MAP(Orientation, getOrientation, setOrientation), MAP(Scale, getScale, setScale));
			}
		}
	}
}
