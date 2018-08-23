#include "../ogrelib.h"

#include "ogrelightmanager.h"

#include "ogrescenerenderer.h"

RegisterClass(Engine::Scene::OgreLightManager);

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
			return VirtualUniqueComponentBase::init();
		}

		std::unique_ptr<Light> OgreLightManager::createLightImpl()
		{
			return std::make_unique<OgreLight>(mSceneMgr->createLight());
		}


	}
}
