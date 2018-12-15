#include "../baselib.h"

#include "scenemanager.h"

#include "Interfaces/serialize/serializableids.h"

#include "entity/entity.h"

#include "Interfaces/generic/transformIt.h"

#include "scenecomponentbase.h"

#include "../app/application.h"

#include "Interfaces/scripting/datatypes/luatablefieldaccessor.h"

#include "Interfaces/scripting/types/api.h"

#include "Interfaces/generic/keyvalueiterate.h"

#include "Interfaces/debug/profiler/profiler.h"


namespace Engine
{

	namespace Scene
	{

		template struct MADGINE_BASE_EXPORT UniqueComponentCollector<SceneComponentBase, SceneManager&>;

		SceneManager::SceneManager(App::Application &app) :
			SerializableUnit(Serialize::SCENE_MANAGER),
			Scope<SceneManager, UniqueComponent<Serialize::NoParentUnit<SceneManager>, App::GlobalAPICollector>>(app),
			mItemCount(0),
			mSceneComponents(*this),
			mApp(app)
		{
		}

		bool SceneManager::init()
		{
			app().addFrameListener(this);
			markInitialized();
			for (const std::unique_ptr<SceneComponentBase>& component : mSceneComponents)
			{
				if (!component->callInit())
					return false;
			}

			return true;
		}

		void SceneManager::finalize()
		{
			clear();

			for (const std::unique_ptr<SceneComponentBase>& component : mSceneComponents)
			{
				component->callFinalize();
			}			
		}

		std::list<Entity::Entity *> SceneManager::entities()
		{
			std::list<Entity::Entity*> result;
			for (Entity::Entity& e : mEntities)
			{
				if (std::find(mEntityRemoveQueue.begin(), mEntityRemoveQueue.end(), &e) == mEntityRemoveQueue.end())
					result.push_back(&e);
			}
			return result;
		}


		SceneComponentBase& SceneManager::getComponent(size_t i, bool init)
		{
			SceneComponentBase &comp = mSceneComponents.get(i);
			if (init)
			{
				checkInitState();
				comp.callInit();
			}
			return comp.getSelf(init);
		}

		size_t SceneManager::getComponentCount()
		{
			return mSceneComponents.size();
		}


		void SceneManager::writeState(Serialize::SerializeOutStream& out) const
		{
			SerializableUnitBase::writeState(out);
		}

		App::GlobalAPIComponentBase & SceneManager::getGlobalAPIComponent(size_t i, bool init)
		{			
			if (init)
			{
				checkInitState();
			}
			return mApp.getGlobalAPIComponent(i, init);
		}

		SceneManager& SceneManager::getSelf(bool init)
		{
			if (init)
			{
				checkDependency();
			}
			return *this;
		}

		void SceneManager::readState(Serialize::SerializeInStream& in)
		{
			clear();

			SerializableUnitBase::readState(in);

			mStateLoadedSignal.emit();
		}

		bool SceneManager::frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, ContextMask mask)
		{
			PROFILE();
				
			for (const std::unique_ptr<SceneComponentBase>& component : mSceneComponents)
			{
				//PROFILE(component->componentName());
				component->update(timeSinceLastFrame, mask);
			}

			removeQueuedEntities();

			return true;
		}

		bool SceneManager::frameFixedUpdate(std::chrono::microseconds timeStep, ContextMask mask)
		{
			{
				//PROFILE("SceneComponents");
				for (const std::unique_ptr<SceneComponentBase>& component : mSceneComponents)
				{
					//PROFILE(component->componentName());
					component->fixedUpdate(timeStep, mask);
				}
			}

			return true;
		}

		Entity::Entity* SceneManager::findEntity(const std::string& name)
		{
			auto it = std::find_if(mEntities.begin(), mEntities.end(), [&](const Entity::Entity& e)
			{
				return e.key() == name;
			});
			if (it == mEntities.end())
			{
				throw 0;
			}
			return &*it;
		}

		KeyValueMapList SceneManager::maps()
		{
			return Scope::maps().merge(mSceneComponents, toPointer(mEntities), MAP_F(findEntity), MAP_RO(MasterId, masterId), MAP_RO(SlaveId, slaveId),
				MAP_RO(Synced, isSynced));
		}

		std::string SceneManager::generateUniqueName()
		{
			return "Madgine_AutoGen_Name_"s + std::to_string(++mItemCount);
		}


		void SceneManager::removeLater(Entity::Entity* e)
		{
			mEntityRemoveQueue.push_back(e);
		}

		App::Application& SceneManager::app(bool init) const
		{
			if (init)
			{
				checkInitState();
			}
			return mApp.getSelf(init);
		}

		void SceneManager::clear()
		{
			mClearedSignal.emit();

			mEntities.clear();
			mLocalEntities.clear();
			mEntityRemoveQueue.clear();
		}


		Entity::Entity* SceneManager::makeLocalCopy(Entity::Entity& e)
		{
			return &mLocalEntities.emplace_back(e, true);
		}

		Entity::Entity* SceneManager::makeLocalCopy(Entity::Entity&& e)
		{
			return &mLocalEntities.emplace_back(std::forward<Entity::Entity>(e), true);
		}

		std::tuple<SceneManager &, bool, std::string> SceneManager::createNonLocalEntityData(const std::string& name)
		{
			return createEntityData(name, false);
		}

		std::tuple<SceneManager &, bool, std::string> SceneManager::createEntityData(
			const std::string& name, bool local)
		{
			std::string actualName = name.empty() ? generateUniqueName() : name;

			return make_tuple(std::ref(*this), local, actualName);
		}

		SignalSlot::SignalStub<const decltype(SceneManager::mEntities)::iterator&, int>& SceneManager::entitiesSignal()
		{
			return mEntities.signal();
		}

		Entity::Entity* SceneManager::createEntity(const std::string& behavior, const std::string& name,
		                                               std::function<void(Entity::Entity&)> init)
		{
			ValueType behaviorTable = app().table()[behavior];
			Scripting::LuaTable table;
			if (behaviorTable.is<Scripting::LuaTable>())
			{
				table = behaviorTable.as<Scripting::LuaTable>();
			}
			else
			{
				LOG_ERROR(Database::message("Behaviour \"", "\" not found!")(behavior));
			}
			mEntities.emplace_tuple_back_init(init, tuple_cat(createEntityData(name, false), std::make_tuple(table)));
			return &mEntities.back();
		}

		Entity::Entity* SceneManager::createLocalEntity(const std::string& behavior, const std::string& name)
		{
			ValueType behaviorTable = app().table()[behavior];
			Scripting::LuaTable table;
			if (behaviorTable.is<Scripting::LuaTable>())
			{
				table = behaviorTable.as<Scripting::LuaTable>();
			}
			else
			{
				LOG_ERROR(Database::message("Behaviour \"", "\" not found!")(behavior));
			}
			const std::tuple<SceneManager &, bool, std::string>& data = createEntityData(name, true);
			return &mLocalEntities.emplace_back(std::get<0>(data), std::get<1>(data), std::get<2>(data), table);
		}

		void SceneManager::removeQueuedEntities()
		{
			std::list<Entity::Entity *>::iterator it = mEntityRemoveQueue.begin();

			auto find = [&](const Entity::Entity& ent) { return &ent == *it; };

			while (it != mEntityRemoveQueue.end())
			{
				if ((*it)->isLocal())
				{
					auto ent = std::find_if(mLocalEntities.begin(), mLocalEntities.end(), find);
					if (ent != mLocalEntities.end())
					{
						mLocalEntities.erase(ent);
					}
					else
					{
						throw 0;
					}
				}
				else
				{
					auto ent = std::find_if(mEntities.begin(), mEntities.end(), find);
					if (ent != mEntities.end())
					{
						mEntities.erase(ent);
					}
					else
					{
						throw 0;
					}
				}

				it = mEntityRemoveQueue.erase(it);
			}
		}
	}
}
