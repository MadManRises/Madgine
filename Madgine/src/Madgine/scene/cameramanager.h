#pragma once

#include "scenecomponent.h"

#include "Interfaces/serialize/container/list.h"

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

			void finalize() override;

			void clear();

		protected:
			virtual std::unique_ptr<Camera> createCameraImpl() = 0;

		private:
			std::vector<std::unique_ptr<Camera>> mCameras;


		};

	}
}

RegisterClass(Engine::Scene::CameraManager);