#include "../../baselib.h"
#include "entitycomponentbase.h"
#include "entity.h"
#include "../scenemanager.h"
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
