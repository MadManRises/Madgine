#pragma once

#include "../../serialize/serializableunit.h"
#include "../../scripting/types/scope.h"

namespace Engine
{
	namespace Scene
	{
		namespace Entity
		{
			class MADGINE_BASE_EXPORT EntityComponentBase : public Serialize::SerializableUnitBase, public Scripting::Scope<EntityComponentBase>
			{
			public:
				EntityComponentBase(Entity& entity, const Scripting::LuaTable& initTable = {});
				virtual ~EntityComponentBase() = 0;

				virtual void init();
				virtual void finalize();

				void moveToEntity(Entity* ent);
				Entity& getEntity() const;

				virtual const char* key() const = 0;

				template <class T>
				T &getSceneComponent(bool init = true)
				{
					return static_cast<T&>(getSceneComponent(T::component_index(), init));
				}

				SceneComponentBase &getSceneComponent(size_t i, bool = true);
				
				template <class T>
				T &getGlobalAPIComponent(bool init = true)
				{
					return static_cast<T&>(getGlobalAPIComponent(T::component_index(), init));
				}

				Scripting::GlobalAPIComponentBase &getGlobalAPIComponent(size_t i, bool = true);

				App::Application &app(bool = true);

				

			private:

				Entity* mEntity;
				Scripting::LuaTable mInitTable;
			};
		}
	}
}
