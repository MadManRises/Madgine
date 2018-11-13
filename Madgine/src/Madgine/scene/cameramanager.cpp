#include "../baselib.h"

#include "cameramanager.h"

#include "camera.h"



namespace Engine
{

	namespace Scene
	{
		CameraManager::~CameraManager()
		{
		}

		Camera* CameraManager::createCamera()
		{
			return mCameras.emplace_back(createCameraImpl()).get();
		}

		const std::vector<std::unique_ptr<Camera>> &CameraManager::cameras() const
		{
			return mCameras;
		}

		void CameraManager::finalize()
		{
			clear();
		}

		void CameraManager::clear()
		{
			mCameras.clear();
		}

	}
}
