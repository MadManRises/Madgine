#include "../../baselib.h"
#include "entitycomponentbase.h"
#include "entity.h"
#include "../scenemanagerbase.h"
#include "../../app/application.h"

namespace Engine
{
	namespace Scene
	{
		namespace Entity
		{
			EntityComponentBase::EntityComponentBase(Entity& entity, const Scripting::LuaTable& initTable) :
				ScopeBase(entity.sceneMgr().app().createTable()),
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

			SceneComponentBase& EntityComponentBase::getSceneComponent(size_t i)
			{
				return mEntity->getSceneComponent(i);
			}

			Scripting::GlobalAPIComponentBase & EntityComponentBase::getGlobalAPIComponent(size_t i)
			{
				return mEntity->getGlobalAPIComponent(i);
			}

			App::Application& EntityComponentBase::app()
			{
				return mEntity->app();
			}

			void EntityComponentBase::moveToEntity(Entity* entity)
			{
				mEntity = entity;
			}
		}
	}
}
