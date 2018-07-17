#pragma once

#include "Madgine/scene/camera.h"

namespace Engine
{
	namespace Scene
	{
		class OgreCamera : public Camera
		{
		public:
			OgreCamera(Ogre::Camera* camera, Ogre::SceneNode *node);
			virtual ~OgreCamera();

			Ogre::Camera *getCamera();

			virtual void setPosition(const Vector3 &pos) override;
			virtual Vector3 getPosition() const override;

		private:
			Ogre::Camera* mCamera;
			Ogre::SceneNode *mNode;
		};
	}
}
