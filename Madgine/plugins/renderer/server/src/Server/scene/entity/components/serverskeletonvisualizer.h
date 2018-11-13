#pragma once


#include "Madgine/scene/entity/entitycomponent.h"

namespace Engine
{
	namespace Scene
	{
		namespace Entity
		{
			class MADGINE_SERVER_EXPORT ServerSkeletonVisualizer : public EntityComponent<ServerSkeletonVisualizer>
			{
			public:
				ServerSkeletonVisualizer(Entity& entity, const Scripting::LuaTable& table = {});
				virtual ~ServerSkeletonVisualizer();
			};
		}
	}
}

RegisterClass(Engine::Scene::Entity::ServerSkeletonVisualizer);