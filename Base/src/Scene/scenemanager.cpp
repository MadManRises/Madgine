#include "baselib.h"

#include "scenemanager.h"

namespace Engine {

	API_IMPL(Scene::SceneManager, &createSceneArray, &createSceneStruct, &createSceneList, &findEntity);

	namespace Scene {
		
		SceneManager::SceneManager() :
			RefScopeTopLevelSerializableUnit(Serialize::SCENE_MANAGER)
		{
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

		void SceneManager::init() {

			MadgineObject::init();

			for (const std::unique_ptr<BaseSceneComponent> &component : mSceneComponents) {
				component->init();
			}
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
				out << component->componentName();
				component->writeState(out);
			}
			out << ValueType::EOL();
		}

		void SceneManager::loadComponentData(Serialize::SerializeInStream &in)
		{
			std::string componentName;
			while (in.loopRead(componentName)) {
				for (const std::unique_ptr<BaseSceneComponent> &component : mSceneComponents) {
					if (component->componentName() == componentName) {
						component->readState(in);
						break;
					}
				}
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
		}


		std::set<BaseSceneComponent*> SceneManager::getComponents()
		{
			std::set<BaseSceneComponent*> result;
			for (const std::unique_ptr<BaseSceneComponent> &e : mSceneComponents) {
				result.insert(e.get());
			}
			return result;
		}

	}
}