#pragma once

#include "Madgine/serialize/container/list.h"


#include "Madgine/scene/light.h"
#include "Madgine/uniquecomponent.h"

#include "Madgine/scene/lightmanager.h"

namespace Engine
{
	namespace Scene
	{
		class MADGINE_SERVER_EXPORT ServerLightManager :
			public VirtualSceneComponentImpl<ServerLightManager, LightManager>
		{
		public:
			ServerLightManager(SceneManager &sceneMgr);
			virtual ~ServerLightManager();

		protected:

			std::unique_ptr<Light> createLightImpl() override;
			
		};
	}
}
