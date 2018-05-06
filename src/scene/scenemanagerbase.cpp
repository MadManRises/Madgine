#include "../baselib.h"

#include "scenemanagerbase.h"

#include "../serialize/toplevelids.h"

#include "entity/entity.h"

#include "../generic/transformIt.h"

#include "scenecomponentbase.h"

#include "../app/application.h"

	API_IMPL(Engine::Scene::SceneManagerBase, MAP_F(findEntity), MAP_RO(MasterId, masterId), MAP_RO(SlaveId, slaveId),
		MAP_RO(Active, isActive));

namespace Engine
{

	namespace Scene
	{
		SceneManagerBase::SceneManagerBase(App::Application &app) :
			SerializableUnit(Serialize::SCENE_MANAGER),
			Scope<Engine::Scene::SceneManagerBase, Engine::Scripting::ScopeBase>(app.createTable()),
			mItemCount(0),
			mSceneComponents(*this),
			mApp(app)
		{
		}

		bool SceneManagerBase::init()
		{
			if (!MadgineObject::init())
				return false;

			for (const std::unique_ptr<SceneComponentBase>& component : mSceneComponents)
			{
				if (!component->init())
					return false;
			}

			return true;
		}

		void SceneManagerBase::finalize()
		{
			clear();

			for (const std::unique_ptr<SceneComponentBase>& component : mSceneComponents)
			{
				component->finalize();
			}

			MadgineObject::finalize();
		}

		std::list<Entity::Entity *> SceneManagerBase::entities()
		{
			std::list<Entity::Entity*> result;
			for (Entity::Entity& e : mEntities)
			{
				if (find(mEntityRemoveQueue.begin(), mEntityRemoveQueue.end(), &e) == mEntityRemoveQueue.end())
					result.push_back(&e);
			}
			return result;
		}


		SceneComponentBase& SceneManagerBase::getComponent(size_t i)
		{
			return mSceneComponents.get(i);
		}

		size_t SceneManagerBase::getComponentCount()
		{
			return mSceneComponents.size();
		}


		void SceneManagerBase::writeState(Serialize::SerializeOutStream& out) const
		{
			SerializableUnitBase::writeState(out);
		}

		void SceneManagerBase::readState(Serialize::SerializeInStream& in)
		{
			clear();

			SerializableUnitBase::readState(in);

			mStateLoadedSignal.emit();
		}

		void SceneManagerBase::update(float timeSinceLastFrame, ContextMask mask)
		{
			{
				//PROFILE("SceneComponents");
				for (const std::unique_ptr<SceneComponentBase>& component : mSceneComponents)
				{
					//PROFILE(component->componentName());
					component->update(timeSinceLastFrame, mask);
				}
			}

			removeQueuedEntities();
		}

		void SceneManagerBase::fixedUpdate(float timeStep, ContextMask mask)
		{
			{
				//PROFILE("SceneComponents");
				for (const std::unique_ptr<SceneComponentBase>& component : mSceneComponents)
				{
					//PROFILE(component->componentName());
					component->fixedUpdate(timeStep, mask);
				}
			}
		}

		Entity::Entity* SceneManagerBase::findEntity(const std::string& name)
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

		KeyValueMapList SceneManagerBase::maps()
		{
			return Scope::maps().merge(mSceneComponents, transformIt<ToPointerConverter>(mEntities));
		}

		std::string SceneManagerBase::generateUniqueName()
		{
			return std::string("Madgine_AutoGen_Name_") + std::to_string(++mItemCount);
		}


		void SceneManagerBase::removeLater(Entity::Entity* e)
		{
			mEntityRemoveQueue.push_back(e);
		}

		const char* SceneManagerBase::key() const
		{
			return "Scene";
		}

		App::Application& SceneManagerBase::app() const
		{
			return mApp;
		}

		void SceneManagerBase::clear()
		{
			mClearedSignal.emit();

			mEntities.clear();
			mLocalEntities.clear();
			mEntityRemoveQueue.clear();
		}


		Entity::Entity* SceneManagerBase::makeLocalCopy(Entity::Entity& e)
		{
			return &mLocalEntities.emplace_back(e, true);
		}

		Entity::Entity* SceneManagerBase::makeLocalCopy(Entity::Entity&& e)
		{
			return &mLocalEntities.emplace_back(std::forward<Entity::Entity>(e), true);
		}

		std::tuple<SceneManagerBase &, bool, std::string> SceneManagerBase::createNonLocalEntityData(const std::string& name)
		{
			return createEntityData(name, false);
		}

		std::tuple<SceneManagerBase &, bool, std::string> SceneManagerBase::createEntityData(
			const std::string& name, bool local)
		{
			std::string actualName = name.empty() ? generateUniqueName() : name;

			return make_tuple(std::ref(*this), local, actualName);
		}

		Entity::Entity* SceneManagerBase::createEntity(const std::string& behaviour, const std::string& name,
		                                               std::function<void(Entity::Entity&)> init)
		{
			mEntities.emplace_tuple_back_init(init, tuple_cat(createEntityData(name, false), make_tuple(behaviour)));
			return &mEntities.back();
		}

		Entity::Entity* SceneManagerBase::createLocalEntity(const std::string& behaviour, const std::string& name)
		{
			const std::tuple<SceneManagerBase &, bool, std::string>& data = createEntityData(name, true);
			return &mLocalEntities.emplace_back(std::get<0>(data), std::get<1>(data), std::get<2>(data), behaviour);
		}

		void SceneManagerBase::removeQueuedEntities()
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
