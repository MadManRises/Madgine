#include "../ogrelib.h"

#include "ogrecamera.h"

#include "Madgine/math/vector3.h"

namespace Engine
{
	namespace Scene
	{
		OgreCamera::OgreCamera(Ogre::Camera* camera, Ogre::SceneNode *node) :
			mCamera(camera),
			mNode(node)
		{
			mNode->setPosition(1, 80, 0);
			mNode->lookAt({ 0,0,0 }, Ogre::Node::TS_WORLD);			

			mCamera->setFarClipDistance(5000);
			mCamera->setNearClipDistance(1);
			mCamera->setAutoAspectRatio(true);
		}

		OgreCamera::~OgreCamera()
		{
			Ogre::SceneManager *sceneMgr = mCamera->getSceneManager();
			sceneMgr->destroySceneNode(mNode);
			sceneMgr->destroyCamera(mCamera);
		}

		Ogre::Camera* OgreCamera::getCamera()
		{
			return mCamera;
		}

		Ogre::SceneNode* OgreCamera::getNode()
		{
			return mNode;
		}

		void OgreCamera::setPosition(const Vector3& pos)
		{
			mNode->setPosition(Ogre::Vector3(pos.ptr()));
		}

		Vector3 OgreCamera::getPosition() const
		{
			return Engine::Vector3(mNode->getPosition().ptr());
		}
	}
}
