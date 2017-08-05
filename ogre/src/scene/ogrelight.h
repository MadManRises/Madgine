#pragma once

#include "Scene\light.h"

namespace Engine {
	namespace Scene {

		class OgreLight : public Light {
		public:
			OgreLight(Serialize::TopLevelSerializableUnitBase *topLevel, Ogre::Light *light);
			virtual ~OgreLight();

		protected:
			virtual void onPositionChanged(const Vector3 &position) override;
			virtual void onPowerChanged(float power) override;

		private:
			Ogre::Light *mLight;
		};

	}
}