#include "../../../baselib.h"

#include "mesh.h"

namespace Engine
{

	API_IMPL(Scene::Entity::Mesh, MAP(Name, getName, setName), MAP(Visible, isVisible, setVisible));


	namespace Scene
	{
		namespace Entity
		{
			
			ENTITYCOMPONENTVIRTUALBASE_IMPL(Mesh, Mesh);

		}
	}
}
