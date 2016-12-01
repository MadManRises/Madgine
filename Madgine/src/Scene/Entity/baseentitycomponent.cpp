#include "madginelib.h"
#include "baseentitycomponent.h"
#include "entity.h"

namespace Engine {
namespace Scene {
namespace Entity {

BaseEntityComponent::BaseEntityComponent(Entity &entity) :
    mEntity(entity)
{
}

BaseEntityComponent::~BaseEntityComponent() {

}

void BaseEntityComponent::finalize() {

}

Entity &BaseEntityComponent::getEntity() const
{
    return mEntity;
}

void BaseEntityComponent::save(Scripting::Serialize::SerializeOutStream &out) const
{

}

void BaseEntityComponent::load(Scripting::Serialize::SerializeInStream &in)
{

}

void BaseEntityComponent::positionChanged(const Ogre::Vector3 &){}

Ogre::Vector2 BaseEntityComponent::getPosition2D()
{
    return mEntity.getPosition2D();
}

const Ogre::Vector3 &BaseEntityComponent::getPosition()
{
    return mEntity.getPosition();
}

Ogre::Vector2 BaseEntityComponent::getCenter2D()
{
	return mEntity.getCenter2D();
}

Ogre::Vector3 BaseEntityComponent::getCenter()
{
	return mEntity.getCenter();
}

const Ogre::Quaternion & BaseEntityComponent::getOrientation()
{
	return mEntity.getOrientation();
}

const Ogre::Vector3 & BaseEntityComponent::getScale()
{
	return mEntity.getScale();
}

}
}
}
