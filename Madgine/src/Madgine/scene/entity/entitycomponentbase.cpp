#include "../../baselib.h"
#include "entitycomponentbase.h"
#include "entity.h"

namespace Engine
{
	API_IMPL(Scene::Entity::EntityComponentBase, MAP_RO(MasterId, masterId), MAP_RO(SlaveId, slaveId),
		MAP_RO(Active, isActive));
	
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

			void EntityComponentBase::moveToEntity(Entity* entity)
			{
				mEntity = entity;
			}
		}
	}
}
