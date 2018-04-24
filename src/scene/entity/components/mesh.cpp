#include "../../../baselib.h"

#include "mesh.h"

namespace Engine
{

	API_IMPL(Scene::Entity::Mesh, MAP_RO(Name, getName), MAP_RO(Visible, isVisible));


	namespace Scene
	{
		namespace Entity
		{
			
			ENTITYCOMPONENTVIRTUALBASE_IMPL(Mesh, Mesh);

		}
	}
}
