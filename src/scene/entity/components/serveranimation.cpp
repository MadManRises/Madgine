#include "../../../serverlib.h"

#include "serveranimation.h"

namespace Engine
{
	API_IMPL(Scene::Entity::Animation);

	namespace Scene
	{
		namespace Entity
		{
			ENTITYCOMPONENT_IMPL(Animation, Animation);

			Animation::Animation(Entity& entity, const Scripting::LuaTable& table) :
				EntityComponent(entity, table),
				mDefaultAnimation(table.getValue("default").asDefault<std::string>(""))
			{
			}

			Animation::~Animation()
			{
			}
		}
	}
}
