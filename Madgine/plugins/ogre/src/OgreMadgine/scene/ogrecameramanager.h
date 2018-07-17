#pragma once


#include "Madgine/scene/cameramanager.h"
#include "Madgine/scene/scenecomponent.h"


namespace Engine
{
	namespace Scene
	{
		class OGREMADGINE_EXPORT OgreCameraManager :
			public VirtualSceneComponentImpl<OgreCameraManager, CameraManager>
		{
		public:
			OgreCameraManager(SceneManager &sceneMgr);
			virtual ~OgreCameraManager();
			
			virtual bool init() override;

		protected:

			std::unique_ptr<Camera> createCameraImpl() override;

		private:

			Ogre::SceneManager *mSceneMgr;

			
		};
	}
}
