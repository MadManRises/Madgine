#include "../../../serverlib.h"

#include "serveranimation.h"

#include "Madgine/generic/valuetype.h"

RegisterClass(Engine::Scene::Entity::Animation);

namespace Engine
{
	

	namespace Scene
	{
		namespace Entity
		{
			ENTITYCOMPONENT_IMPL(Animation, Animation);

			Animation::Animation(Entity& entity, const Scripting::LuaTable& table) :
				EntityComponent(entity, table),
				mDefaultAnimation(table["default"].asDefault<std::string>(""))
			{
			}

			Animation::~Animation()
			{
			}
		}
	}
}
