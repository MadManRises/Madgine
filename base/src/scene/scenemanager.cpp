#include "baselib.h"

#include "scenemanager.h"

#include "serialize/toplevelids.h"

#include "entity/entity.h"

namespace Engine {

	API_IMPL(Engine::Scene::SceneManagerBase, MAP_F(findEntity), MAP_RO(MasterId, masterId), MAP_RO(SlaveId, slaveId), MAP_RO(Active, isActive));

#ifdef _MSC_VER
	template class MADGINE_BASE_EXPORT Scene::SceneComponentCollector;
#endif


	namespace Scene {
		
		SceneManagerBase::SceneManagerBase() :
			SerializableUnit(Serialize::SCENE_MANAGER),
			mItemCount(0),
			mSceneComponents(this)
		{
		}

		bool SceneManagerBase::init() {

			if (!MadgineObject::init())
				return false;

			for (const std::unique_ptr<SceneComponentBase> &component : mSceneComponents) {
				if (!component->init())
					return false;
			}

			return true;
		}

		void SceneManagerBase::finalize() {
			
			clear();

			for (const std::unique_ptr<SceneComponentBase> &component : mSceneComponents) {
				component->finalize();
			}

			MadgineObject::finalize();
		}

		std::list<Entity::Entity *> SceneManagerBase::entities()
		{
			std::list<Entity::Entity*> result;
			for (Entity::Entity &e : mEntities) {
				if (std::find(mEntityRemoveQueue.begin(), mEntityRemoveQueue.end(), &e) == mEntityRemoveQueue.end())
					result.push_back(&e);
			}
			return result;
		}


		size_t SceneManagerBase::getComponentCount()
		{
			return mSceneComponents.size();
		}


		

		void SceneManagerBase::writeState(Serialize::SerializeOutStream &out) const
		{
			Serialize::SerializableUnitBase::writeState(out);
		}

		void SceneManagerBase::readState(Serialize::SerializeInStream &in)
		{
			clear();

			Serialize::SerializableUnitBase::readState(in);

			mStateLoadedSignal.emit();
		}

		void SceneManagerBase::update(float timeSinceLastFrame, ContextMask mask) {
			{
				//PROFILE("SceneComponents");
				for (const std::unique_ptr<SceneComponentBase> &component : mSceneComponents) {
					//PROFILE(component->componentName());
					component->update(timeSinceLastFrame, mask);
				}
			}

			removeQueuedEntities();
		}

		void SceneManagerBase::fixedUpdate(float timeStep, ContextMask mask) {
			{
				//PROFILE("SceneComponents");
				for (const std::unique_ptr<SceneComponentBase> &component : mSceneComponents) {
					//PROFILE(component->componentName());
					component->fixedUpdate(timeStep, mask);
				}
			}
		}

		Entity::Entity * SceneManagerBase::findEntity(const std::string & name)
		{
			auto it = std::find_if(mEntities.begin(), mEntities.end(), [&](const Entity::Entity &e) {
				return e.key() == name;
			});
			if (it == mEntities.end()) {
				throw 0;
			}
			else
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


		void SceneManagerBase::removeLater(Entity::Entity *e)
		{
			mEntityRemoveQueue.push_back(e);
		}

		const char *SceneManagerBase::key() const
		{
			return "Scene";
		}

		void SceneManagerBase::clear() {
			mClearedSignal.emit();

			mEntities.clear();
			mLocalEntities.clear();		
			mEntityRemoveQueue.clear();
		}


		void SceneManagerBase::makeLocalCopy(Entity::Entity & e)
		{
			mLocalEntities.emplace_back(e);
		}

		void SceneManagerBase::makeLocalCopy(Entity::Entity && e)
		{
			mLocalEntities.emplace_back(std::forward<Entity::Entity>(e));
		}

		std::tuple<SceneManagerBase *, std::string> SceneManagerBase::createEntityData(const std::string & name)
		{
			std::string actualName = name.empty() ? generateUniqueName() : name;

			return std::make_tuple(this, actualName);
		}

		Entity::Entity *SceneManagerBase::createEntity(const std::string &behaviour, const std::string &name, std::function<void(Entity::Entity&)> init)
		{
			mEntities.emplace_tuple_back_init(init, std::tuple_cat(createEntityData(name), std::make_tuple(behaviour)));
			return &mEntities.back();
		}

		Entity::Entity * SceneManagerBase::createLocalEntity(const std::string & behaviour, const std::string & name)
		{
			const std::tuple<SceneManagerBase *, std::string> &data = createEntityData(name);
			mLocalEntities.emplace_back(std::get<0>(data), std::get<1>(data), behaviour);
			return &mLocalEntities.back();
		}

		void SceneManagerBase::removeQueuedEntities()
		{
			std::list<Entity::Entity *>::iterator it = mEntityRemoveQueue.begin();

			auto find = [&](const Entity::Entity &ent) {return &ent == *it; };

			while (it != mEntityRemoveQueue.end()) {

				auto ent = std::find_if(mEntities.begin(), mEntities.end(), find);
				if (ent != mEntities.end()) {
					mEntities.erase(ent);
				}
				else {
					auto ent = std::find_if(mLocalEntities.begin(), mLocalEntities.end(), find);
					if (ent != mLocalEntities.end()) {
						mLocalEntities.erase(ent);
					}
					else {
						throw 0;
					}
				}

				it = mEntityRemoveQueue.erase(it);
			}
		}

	}
}