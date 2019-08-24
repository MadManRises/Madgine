#include "../../baselib.h"
#include "entitycomponentbase.h"
#include "entity.h"


#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"

#include "Modules/serialize/serializetable_impl.h"

namespace Engine
{
	

	
	namespace Scene
	{
		namespace Entity
		{
                EntityComponentBase::EntityComponentBase(Entity &entity /*, const Scripting::LuaTable& initTable*/)
                    : 
				mEntity(&entity)/*,
				mInitTable(initTable)*/
			{
			}

			EntityComponentBase::~EntityComponentBase()
			{
			}

			void EntityComponentBase::init()
			{
			}

			void EntityComponentBase::finalize()
			{
			}

			Entity& EntityComponentBase::getEntity() const
			{
				return *mEntity;
			}

			EntityComponentBase* EntityComponentBase::getComponent(const std::string& name)
			{
				return mEntity->getComponent(name);
			}

			EntityComponentBase* EntityComponentBase::addComponent(const std::string& name/*, const Scripting::LuaTable& init*/)
			{
				return mEntity->addComponent(name/*, init*/);
			}

			SceneComponentBase& EntityComponentBase::getSceneComponent(size_t i, bool init)
			{
				return mEntity->getSceneComponent(i, init);
			}

			App::GlobalAPIBase & EntityComponentBase::getGlobalAPIComponent(size_t i, bool init)
			{
				return mEntity->getGlobalAPIComponent(i, init);
			}

			App::Application& EntityComponentBase::app(bool init)
			{
				return mEntity->app(init);
			}

			/*KeyValueMapList EntityComponentBase::maps()
			{
				return Scope::maps().merge(MAP_RO(MasterId, masterId), MAP_RO(SlaveId, slaveId),
					MAP_RO(Synced, isSynced));
			}*/

			/*const Scripting::LuaTable & EntityComponentBase::initTable()
			{
				return mInitTable;
			}*/

			void EntityComponentBase::moveToEntity(Entity* entity)
			{
				mEntity = entity;
			}
		}
	}
}

METATABLE_BEGIN(Engine::Scene::Entity::EntityComponentBase)
METATABLE_END(Engine::Scene::Entity::EntityComponentBase)


SERIALIZETABLE_BEGIN(Engine::Scene::Entity::EntityComponentBase)
SERIALIZETABLE_END(Engine::Scene::Entity::EntityComponentBase)

RegisterType(Engine::Scene::Entity::EntityComponentBase);