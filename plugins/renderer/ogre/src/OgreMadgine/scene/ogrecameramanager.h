#pragma once


#include "Madgine/scene/cameramanager.h"


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

		protected:

			virtual bool init() override;

			std::unique_ptr<Camera> createCameraImpl() override;

		private:

			Ogre::SceneManager *mSceneMgr;

			
		};
	}
}
