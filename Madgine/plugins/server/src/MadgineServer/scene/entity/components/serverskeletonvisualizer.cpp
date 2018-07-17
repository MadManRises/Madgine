#include "../../../serverlib.h"

#include "servermesh.h"

#include "serverskeletonvisualizer.h"

namespace Engine
{
	API_IMPL(Scene::Entity::ServerSkeletonVisualizer);

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
