#include "../../../baselib.h"

#include "animation.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"

namespace Engine
{

	namespace Scene
	{
		namespace Entity
		{

			void Animation::resetDefaultAnimation()
			{
				setDefaultAnimation("");
			}

		}
	}
}

ENTITYCOMPONENTVIRTUALBASE_IMPL(Animation, Engine::Scene::Entity::Animation);


METATABLE_BEGIN(Engine::Scene::Entity::Animation)
METATABLE_END(Engine::Scene::Entity::Animation)

RegisterType(Engine::Scene::Entity::Animation);