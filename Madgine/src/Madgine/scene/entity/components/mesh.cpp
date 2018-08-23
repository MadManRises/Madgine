#include "../../../baselib.h"

#include "mesh.h"

#include "../../../scripting/types/api.h"

#include "../../../generic/keyvalueiterate.h"

RegisterClass(Engine::Scene::Entity::Mesh);

namespace Engine
{




	namespace Scene
	{
		namespace Entity
		{
			
			ENTITYCOMPONENTVIRTUALBASE_IMPL(Mesh, Mesh);

			KeyValueMapList Mesh::maps()
			{
				return Scope::maps().merge(MAP(Name, getName, setName), MAP(Visible, isVisible, setVisible));
			}
		}
	}
}
