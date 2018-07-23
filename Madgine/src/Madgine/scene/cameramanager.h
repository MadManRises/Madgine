#pragma once

#include "scenecomponent.h"

#include "../serialize/container/list.h"

#include "light.h"

namespace Engine
{
	namespace Scene
	{


		class MADGINE_CLIENT_EXPORT CameraManager : public VirtualSceneComponentBase<CameraManager>
		{
		public:
			using VirtualSceneComponentBase<CameraManager>::VirtualSceneComponentBase;
			virtual ~CameraManager();

			Camera* createCamera();
			const std::vector<std::unique_ptr<Camera>> &cameras() const;

			void clear();

		protected:
			virtual std::unique_ptr<Camera> createCameraImpl() = 0;

		private:
			std::vector<std::unique_ptr<Camera>> mCameras;


		};

	}
}