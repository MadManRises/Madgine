#pragma once


#include "../entitycomponent.h"
#include "../../../serialize/container/serialized.h"

namespace Engine
{
	namespace Scene
	{
		namespace Entity
		{
			class MADGINE_SERVER_EXPORT Animation : public EntityComponent<Animation>
			{
			public:
				Animation(Entity& entity, const Scripting::LuaTable& table);
				virtual ~Animation();

			private:
				Serialize::Serialized<std::string> mDefaultAnimation;
			};
		}
	}
}
