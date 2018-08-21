#pragma once

#include "scenecomponent.h"

#include "../serialize/container/list.h"

#include "light.h"

namespace Engine
{
	namespace Scene
	{
		

		class MADGINE_BASE_EXPORT LightManager : public VirtualSceneComponentBase<LightManager>
		{
		public:
			using VirtualSceneComponentBase<LightManager>::VirtualSceneComponentBase;
			virtual ~LightManager();

			Light* createLight();
			const std::list<std::unique_ptr<Light>> &lights() const;

			void finalize() override;
			void clear();

			virtual std::unique_ptr<Light> createLightImpl() = 0;

		private:
			std::tuple<std::unique_ptr<Light>> createLightTuple();

		private:
			Serialize::ObservableList<std::unique_ptr<Light>, Serialize::ContainerPolicies::masterOnly, Serialize::ParentCreator<
				&LightManager::createLightTuple>>
				mLights;


		};

	}
}
