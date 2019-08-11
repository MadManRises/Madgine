#include "../../../serverlib.h"

#include "servermesh.h"

#include "serverskeletonvisualizer.h"


#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"


namespace Engine
{
	

	namespace Scene
	{
		namespace Entity
		{
			ENTITYCOMPONENT_IMPL(SkeletonVisualizer, ServerSkeletonVisualizer);

			ServerSkeletonVisualizer::ServerSkeletonVisualizer(Entity& entity/*, const Scripting::LuaTable& table*/) :
				EntityComponent(entity/*, table*/)
			{
			}


			ServerSkeletonVisualizer::~ServerSkeletonVisualizer()
			{
			}
		}
	}
}


METATABLE_BEGIN(Engine::Scene::Entity::ServerSkeletonVisualizer)
METATABLE_END(Engine::Scene::Entity::ServerSkeletonVisualizer)

RegisterType(Engine::Scene::Entity::ServerSkeletonVisualizer);