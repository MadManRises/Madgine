#include "../../baselib.h"
#include "entitycomponentbase.h"
#include "entity.h"

#include "../../scripting/types/api.h"
#include "../../generic/keyvalueiterate.h"

RegisterClass(Engine::Scene::Entity::EntityComponentBase);

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

			SceneComponentBase& EntityComponentBase::getSceneComponent(size_t i, bool init)
			{
				return mEntity->getSceneComponent(i, init);
			}

			Scripting::GlobalAPIComponentBase & EntityComponentBase::getGlobalAPIComponent(size_t i, bool init)
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
					MAP_RO(Active, isActive));
			}

			void EntityComponentBase::moveToEntity(Entity* entity)
			{
				mEntity = entity;
			}
		}
	}
}
