#include "baselib.h"

#include "scenemanager.h"

#include "serialize/toplevelids.h"

#include "entity/entity.h"

namespace Engine {

	API_IMPL(Engine::Scene::SceneManagerBase, MAP_F(findEntity), MAP_RO(MasterId, masterId), MAP_RO(SlaveId, slaveId));

	namespace Scene {
		
		SceneManagerBase::SceneManagerBase() :
			TopLevelSerializableUnit(Serialize::SCENE_MANAGER),
			mItemCount(0)
		{
			for (const std::unique_ptr<SceneComponentBase> &comp : mSceneComponents) {
				Serialize::UnitHelper<SceneComponentBase>::setItemTopLevel(*comp, this);
			}
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


		size_t SceneManagerBase::getComponentCount()
		{
			return mSceneComponents.size();
		}


		void SceneManagerBase::saveComponentData(Serialize::SerializeOutStream &out) const
		{
			for (const std::unique_ptr<SceneComponentBase> &component : mSceneComponents) {
				out << component->getName();
				component->writeId(out);
				component->writeState(out);
			}
			out << ValueType::EOL();
		}

		void SceneManagerBase::loadComponentData(Serialize::SerializeInStream &in)
		{
			std::string componentName;
			while (in.loopRead(componentName)) {
				auto it = std::find_if(mSceneComponents.begin(), mSceneComponents.end(), [&](const std::unique_ptr<SceneComponentBase> &comp) {return comp->getName() == componentName; });
				(*it)->readId(in);
				(*it)->readState(in);
			}
		}

		void SceneManagerBase::writeState(Serialize::SerializeOutStream &out) const
		{
			saveComponentData(out);

			Serialize::SerializableUnitBase::writeState(out);
		}

		void SceneManagerBase::readState(Serialize::SerializeInStream &in)
		{
			loadComponentData(in);

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

		Scripting::KeyValueMapList SceneManagerBase::maps()
		{
			return Scope::maps().merge(mSceneComponents);
		}

		std::string SceneManagerBase::generateUniqueName()
		{
			return std::string("Madgine_AutoGen_Name_") + std::to_string(++mItemCount);
		}


		void SceneManagerBase::removeLater(Entity::Entity *e)
		{
			mEntityRemoveQueue.push_back(e);
		}

	}
}