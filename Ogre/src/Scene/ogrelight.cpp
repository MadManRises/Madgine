#include "madginelib.h"

#include "ogrelight.h"

Engine::Scene::OgreLight::OgreLight(Ogre::Light * light) :
	mLight(light)
{
}

Engine::Scene::OgreLight::~OgreLight()
{
	mLight->_getCreator()->destroyInstance(mLight);
}

void Engine::Scene::OgreLight::onPositionChanged(const std::array<float, 3>& position)
{
	mLight->setPosition(Ogre::Vector3(position.data()));
}

void Engine::Scene::OgreLight::onPowerChanged(float power)
{
	mLight->setPowerScale(power);
}
