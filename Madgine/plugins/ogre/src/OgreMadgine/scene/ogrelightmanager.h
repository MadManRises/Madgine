#pragma once


#include "Madgine/serialize/container/list.h"

#include "Madgine/scene/lightmanager.h"
#include "Madgine/scene/scenecomponent.h"

#include "ogrelight.h"

namespace Engine
{
	namespace Scene
	{
		class OGREMADGINE_EXPORT OgreLightManager :
			public VirtualSceneComponentImpl<OgreLightManager, LightManager>
		{
		public:
			OgreLightManager(SceneManager &sceneMgr);
			virtual ~OgreLightManager();		

		protected:

			virtual bool init() override;

			std::unique_ptr<Light> createLightImpl() override;

		private:

			Ogre::SceneManager *mSceneMgr;

			
		};
	}
}
