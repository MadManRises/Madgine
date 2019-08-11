#pragma once

#include "Modules/serialize/serializableunit.h"
#include "Modules/uniquecomponent/uniquecomponent.h"
#include "Modules/keyvalue/scopebase.h"
#include "Modules/scripting/datatypes/luatable.h"

namespace Engine
{
	namespace Scene
	{
		namespace Entity
		{
			class MADGINE_BASE_EXPORT EntityComponentBase : public Serialize::SerializableUnitBase, public ScopeBase
			{
			public:
				EntityComponentBase(Entity& entity/*, const Scripting::LuaTable& initTable = {}*/);
				virtual ~EntityComponentBase() = 0;

				virtual void init();
				virtual void finalize();

				void moveToEntity(Entity* ent);
				Entity& getEntity() const;

				virtual const char* key() const = 0;

				template <class T>
				T *getComponent()
				{
					return static_cast<T*>(getComponent(T::componentName()));
				}

				EntityComponentBase* getComponent(const std::string& name);

				template <class T>
				T *addComponent(const Scripting::LuaTable &init = {})
				{
					return static_cast<T*>(addComponent(T::componentName(), init));
				}

				EntityComponentBase* addComponent(const std::string &name/*, const Scripting::LuaTable &init*/);

				template <class T>
				T &getSceneComponent(bool init = true)
				{
					return static_cast<T&>(getSceneComponent(T::component_index, init));
				}

				SceneComponentBase &getSceneComponent(size_t i, bool = true);
				
				template <class T>
				T &getGlobalAPIComponent(bool init = true)
				{
					return static_cast<T&>(getGlobalAPIComponent(T::component_index, init));
				}

				App::GlobalAPIBase &getGlobalAPIComponent(size_t i, bool = true);

				App::Application &app(bool = true);

				//KeyValueMapList maps() override;
				

			protected:
				//const Scripting::LuaTable &initTable();

			private:

				Entity* mEntity;
				//Scripting::LuaTable mInitTable;
			};
		}
	}
}