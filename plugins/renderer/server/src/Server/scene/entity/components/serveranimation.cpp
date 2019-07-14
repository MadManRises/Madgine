#include "../../../serverlib.h"

#include "serveranimation.h"

#include "Modules/keyvalue/valuetype.h"


#include "Modules/keyvalue/metatable.h"
#include "Modules/reflection/classname.h"



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

RegisterType(Engine::Scene::Entity::Animation);