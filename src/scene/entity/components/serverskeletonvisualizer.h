#pragma once


#include "../entitycomponent.h"

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
