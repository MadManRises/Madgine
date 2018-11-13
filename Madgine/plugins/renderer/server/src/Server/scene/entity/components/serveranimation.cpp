#include "../../../serverlib.h"

#include "serveranimation.h"

#include "Interfaces/generic/valuetype.h"


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

