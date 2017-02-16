#include "baselib.h"

#include "scenemanager.h"

#include "TopLevelIds.h"

namespace Engine {

	API_IMPL(Scene::SceneManager, &createSceneArray, &createSceneStruct, &createSceneList, &findEntity);

	namespace Scene {
		
		SceneManager::SceneManager() :
			RefScopeTopLevelSerializableUnit(SCENE_MANAGER),
			mItemCount(0)
		{
			for (const std::unique_ptr<BaseSceneComponent> &comp : mSceneComponents) {
				Serialize::UnitHelper<BaseSceneComponent>::setItemTopLevel(*comp, this);
			}
		}


		Scripting::Scope * SceneManager::createSceneArray(size_t size)
		{
			Scripting::Vector *v = createVector();
			v->resize(size);
			return v;
		}

		Scripting::Scope * SceneManager::createSceneStruct()
		{
			return createStruct();
		}

		Scripting::Scope * SceneManager::createSceneList()
		{
			return createList();
		}

		bool SceneManager::init() {

			if (!MadgineObject::init())
				return false;

			for (const std::unique_ptr<BaseSceneComponent> &component : mSceneComponents) {
				if (!component->init())
					return false;
			}
			return true;
		}

		void SceneManager::finalize() {

			MadgineObject::finalize();

			clear();

			for (const std::unique_ptr<BaseSceneComponent> &component : mSceneComponents) {
				component->finalize();
			}
		}


		size_t SceneManager::getComponentCount()
		{
			return mSceneComponents.size();
		}


		void SceneManager::saveComponentData(Serialize::SerializeOutStream &out) const
		{
			for (const std::unique_ptr<BaseSceneComponent> &component : mSceneComponents) {
				out << component->getName();
				component->writeState(out);
			}
			out << ValueType::EOL();
		}

		void SceneManager::loadComponentData(Serialize::SerializeInStream &in)
		{
			std::string componentName;
			while (in.loopRead(componentName)) {
				auto it = std::find_if(mSceneComponents.begin(), mSceneComponents.end(), [&](const std::unique_ptr<BaseSceneComponent> &comp) {return comp->getName() == componentName; });
				if (it == mSceneComponents.end())
					throw 0;
				(*it)->readState(in);
			}
		}

		void SceneManager::update(float timeSinceLastFrame, App::ContextMask mask) {
			{
				//PROFILE("SceneComponents");
				for (const std::unique_ptr<BaseSceneComponent> &component : mSceneComponents) {
					//PROFILE(component->componentName());
					component->update(timeSinceLastFrame, mask);
				}
			}

			removeQueuedEntities();
		}


		std::string SceneManager::generateUniqueName()
		{
			return std::string("Madgine_AutoGen_Name_") + std::to_string(++mItemCount);
		}


		void SceneManager::removeLater(Entity::Entity *e)
		{
			mEntityRemoveQueue.push_back(e);
		}


	}
}