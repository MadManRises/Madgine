#include "../ogrelib.h"

#include "ogrecameramanager.h"

#include "ogrescenerenderer.h"

#include "ogrecamera.h"

RegisterClass(Engine::Scene::OgreCameraManager);

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
			return VirtualUniqueComponentBase::init();
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
