#include "baselib.h"
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

void BaseEntityComponent::preDelete() {

}

Entity &BaseEntityComponent::getEntity() const
{
    return mEntity;
}


std::array<float, 2> BaseEntityComponent::getPosition2D()
{
    return mEntity.getPosition2D();
}

void BaseEntityComponent::setPosition(const std::array<float, 3> & pos)
{
	mEntity.setPosition(pos);
}

std::array<float, 3> BaseEntityComponent::getPosition()
{
    return mEntity.getPosition();
}

std::array<float, 2> BaseEntityComponent::getCenter2D()
{
	return mEntity.getCenter2D();
}

std::array<float, 3> BaseEntityComponent::getCenter()
{
	return mEntity.getCenter();
}

std::array<float, 4> BaseEntityComponent::getOrientation()
{
	return mEntity.getOrientation();
}

void BaseEntityComponent::setScale(const std::array<float, 3> & scale)
{
	mEntity.setScale(scale);
}

std::array<float, 3> BaseEntityComponent::getScale()
{
	return mEntity.getScale();
}

}
}
}
