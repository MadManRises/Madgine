#include "../../../baselib.h"

#include "transform.h"

namespace Engine
{

	API_IMPL(Scene::Entity::Transform, MAP(Position, getPosition, setPosition), MAP(Orientation, getOrientation, setOrientation), MAP(Scale, getScale, setScale));


	namespace Scene
	{
		namespace Entity
		{
			
			ENTITYCOMPONENTVIRTUALBASE_IMPL(Transform, Transform);

		}
	}
}
