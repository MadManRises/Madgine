#include "ogrelib.h"

#include "ogrelight.h"

namespace Engine {
	namespace Scene {


OgreLight::OgreLight(SceneManagerBase *sceneMgr, Ogre::Light * light) :
	Light(sceneMgr),
	mLight(light)
{
}

OgreLight::~OgreLight()
{
	mLight->_getManager()->destroyLight(mLight);
}

void OgreLight::onPositionChanged(const Vector3& position)
{
	mLight->setPosition(Ogre::Vector3(position.ptr()));
}

void OgreLight::onPowerChanged(float power)
{
	mLight->setPowerScale(power);
}

	}
}