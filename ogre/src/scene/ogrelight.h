#pragma once

#include "scene/light.h"

namespace Engine {
	namespace Scene {

		class OgreLight : public Light {
		public:
			OgreLight(SceneManagerBase *sceneMgr, Ogre::Light *light);
			virtual ~OgreLight();

		protected:
			virtual void onPositionChanged(const Vector3 &position) override;
			virtual void onPowerChanged(float power) override;

		private:
			Ogre::Light *mLight;
		};

	}
}