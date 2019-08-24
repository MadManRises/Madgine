#include "../../../baselib.h"

#include "mesh.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"
#include "Modules/serialize/serializetable_impl.h"

namespace Engine
{
	namespace Scene
	{
		namespace Entity
		{

			/*KeyValueMapList Mesh::maps()
			{
				return Scope::maps().merge(MAP(Name, getName, setName), MAP(Visible, isVisible, setVisible));
			}*/
		}
	}
}

ENTITYCOMPONENTVIRTUALBASE_IMPL(Mesh, Engine::Scene::Entity::Mesh);

METATABLE_BEGIN(Engine::Scene::Entity::Mesh)
METATABLE_END(Engine::Scene::Entity::Mesh)


SERIALIZETABLE_BEGIN(Engine::Scene::Entity::Mesh)
SERIALIZETABLE_END(Engine::Scene::Entity::Mesh)

RegisterType(Engine::Scene::Entity::Mesh);
