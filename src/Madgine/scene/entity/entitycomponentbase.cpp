#include "../../baselib.h"
#include "entitycomponentbase.h"
#include "entity.h"

#include "Interfaces/scripting/types/api.h"
#include "Interfaces/generic/keyvalueiterate.h"



namespace Engine
{
	

	
	namespace Scene
	{
		namespace Entity
		{
			EntityComponentBase::EntityComponentBase(Entity& entity, const Scripting::LuaTable& initTable) :
				Scope(&entity),
				mEntity(&entity),
				mInitTable(initTable)
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

			EntityComponentBase* EntityComponentBase::addComponent(const std::string& name, const Scripting::LuaTable& init)
			{
				return mEntity->addComponent(name, init);
			}

			SceneComponentBase& EntityComponentBase::getSceneComponent(size_t i, bool init)
			{
				return mEntity->getSceneComponent(i, init);
			}

			App::GlobalAPIComponentBase & EntityComponentBase::getGlobalAPIComponent(size_t i, bool init)
			{
				return mEntity->getGlobalAPIComponent(i, init);
			}

			App::Application& EntityComponentBase::app(bool init)
			{
				return mEntity->app(init);
			}

			KeyValueMapList EntityComponentBase::maps()
			{
				return Scope::maps().merge(MAP_RO(MasterId, masterId), MAP_RO(SlaveId, slaveId),
					MAP_RO(Synced, isSynced));
			}

			void EntityComponentBase::moveToEntity(Entity* entity)
			{
				mEntity = entity;
			}
		}
	}
}
