#pragma once

#include "scene/light.h"

namespace Engine
{
	namespace Scene
	{
		class OgreLight : public Light
		{
		public:
			OgreLight(Ogre::Light* light);
			virtual ~OgreLight();

		protected:
			void onPositionChanged(const Vector3& position) override;
			void onPowerChanged(float power) override;

		private:
			Ogre::Light* mLight;
		};
	}
}
