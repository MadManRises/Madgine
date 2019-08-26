#pragma once

#include "Modules/serialize/serializableunit.h"
#include "Modules/uniquecomponent/uniquecomponent.h"
#include "Modules/keyvalue/scopebase.h"

#include "Modules/keyvalue/objectptr.h"

namespace Engine
{
	namespace Scene
	{
		namespace Entity
		{
			class MADGINE_BASE_EXPORT EntityComponentBase : public Serialize::SerializableUnitBase, public ScopeBase
			{
			public:
                            EntityComponentBase(Entity &entity, const ObjectPtr &initTable = {});                            
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
                                T *addComponent(const ObjectPtr &init = {})
				{
					return static_cast<T*>(addComponent(T::componentName(), init));
				}

				EntityComponentBase *addComponent(const std::string &name, const ObjectPtr &init);

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


			protected:
                                const ObjectPtr &initTable();

			private:

				Entity* mEntity;
				ObjectPtr mInitTable;
			};
		}
	}
}