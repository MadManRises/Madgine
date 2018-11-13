#include "../ogrelib.h"

#include "ogrelightmanager.h"

#include "ogrescenerenderer.h"

namespace Engine
{

	namespace Scene
	{
		OgreLightManager::OgreLightManager(SceneManager &sceneMgr) :
			VirtualSceneComponentImpl<Engine::Scene::OgreLightManager, Engine::Scene::LightManager>(sceneMgr)
		{
			
		}

		OgreLightManager::~OgreLightManager()
		{
		}

		bool OgreLightManager::init()
		{
			mSceneMgr = getGlobalAPIComponent<OgreSceneRenderer>().getSceneManager();
			return VirtualSceneComponentImpl<Engine::Scene::OgreLightManager, Engine::Scene::LightManager>::init();
		}

		std::unique_ptr<Light> OgreLightManager::createLightImpl()
		{
			return std::make_unique<OgreLight>(mSceneMgr->createLight());
		}


	}
}


RegisterClass(Engine::Scene::OgreLightManager);