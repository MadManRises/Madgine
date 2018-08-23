#include "../../../serverlib.h"

#include "servermesh.h"

#include "serverskeletonvisualizer.h"

RegisterClass(Engine::Scene::Entity::ServerSkeletonVisualizer);

namespace Engine
{
	

	namespace Scene
	{
		namespace Entity
		{
			ENTITYCOMPONENT_IMPL(SkeletonVisualizer, ServerSkeletonVisualizer);

			ServerSkeletonVisualizer::ServerSkeletonVisualizer(Entity& entity, const Scripting::LuaTable& table) :
				EntityComponent(entity, table)
			{
			}


			ServerSkeletonVisualizer::~ServerSkeletonVisualizer()
			{
			}
		}
	}
}
