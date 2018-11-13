#include "../ogrelib.h"

#include "ogrecameramanager.h"

#include "ogrescenerenderer.h"

#include "ogrecamera.h"

namespace Engine
{

	namespace Scene
	{
		OgreCameraManager::OgreCameraManager(SceneManager &sceneMgr) :
			VirtualSceneComponentImpl<Engine::Scene::OgreCameraManager, Engine::Scene::CameraManager>(sceneMgr)
		{
			
		}

		OgreCameraManager::~OgreCameraManager()
		{
		}

		bool OgreCameraManager::init()
		{
			mSceneMgr = getGlobalAPIComponent<OgreSceneRenderer>().getSceneManager();
			return VirtualSceneComponentImpl<Engine::Scene::OgreCameraManager, Engine::Scene::CameraManager>::init();
		}

		std::unique_ptr<Camera> OgreCameraManager::createCameraImpl()
		{

			Ogre::SceneNode *node = mSceneMgr->createSceneNode();
			Ogre::Camera *camera = mSceneMgr->createCamera("Cam");
			node->attachObject(camera);

			return std::make_unique<OgreCamera>(camera, node);
		}


	}
}


RegisterClass(Engine::Scene::OgreCameraManager);