#include "madginelib.h"

#include "ogrelight.h"

Engine::Scene::OgreLight::OgreLight(Ogre::Light * light) :
	mLight(light)
{
}

Engine::Scene::OgreLight::~OgreLight()
{
	mLight->_getManager()->destroyLight(mLight);
}

void Engine::Scene::OgreLight::onPositionChanged(const Vector3& position)
{
	mLight->setPosition(Ogre::Vector3(position.ptr()));
}

void Engine::Scene::OgreLight::onPowerChanged(float power)
{
	mLight->setPowerScale(power);
}
