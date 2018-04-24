#include "../../../baselib.h"

#include "transform.h"

namespace Engine
{

	API_IMPL(Scene::Entity::Transform, MAP_RO(Position, getPosition), MAP_RO(Orientation, getOrientation), MAP_RO(Scale,
		getScale));


	namespace Scene
	{
		namespace Entity
		{
			
			ENTITYCOMPONENTVIRTUALBASE_IMPL(Transform, Transform);

		}
	}
}
