#include "ogrelib.h"

#include "ogrelight.h"

namespace Engine {
	namespace Scene {


OgreLight::OgreLight(Serialize::TopLevelSerializableUnitBase *topLevel, Ogre::Light * light) :
	Light(topLevel),
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