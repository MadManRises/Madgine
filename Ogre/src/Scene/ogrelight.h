#pragma once

#include "Scene\light.h"

namespace Engine {
	namespace Scene {

		class OgreLight : public Light {
		public:
			OgreLight(Ogre::Light *light);
			virtual ~OgreLight();

		protected:
			virtual void onPositionChanged(const std::array<float, 3> &position) override;
			virtual void onPowerChanged(float power) override;

		private:
			Ogre::Light *mLight;
		};

	}
}