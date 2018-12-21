#pragma once

#include "Interfaces/serialize/serializableunit.h"
#include "Interfaces/scripting/types/scope.h"
#include "../../uniquecomponent/uniquecomponent.h"

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

				virtual const char* key() const override = 0;

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

				EntityComponentBase* addComponent(const std::string &name, const Scripting::LuaTable &init);

				template <class T>
				T &getSceneComponent(bool init = true)
				{
					return static_cast<T&>(getSceneComponent(component_index<T>(), init));
				}

				SceneComponentBase &getSceneComponent(size_t i, bool = true);
				
				template <class T>
				T &getGlobalAPIComponent(bool init = true)
				{
					return static_cast<T&>(getGlobalAPIComponent(component_index<T>(), init));
				}

				App::GlobalAPIComponentBase &getGlobalAPIComponent(size_t i, bool = true);

				App::Application &app(bool = true);

				KeyValueMapList maps() override;
				

			private:

				Entity* mEntity;
				Scripting::LuaTable mInitTable;
			};
		}
	}
}

RegisterType(Engine::Scene::Entity::EntityComponentBase);