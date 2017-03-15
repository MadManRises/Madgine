#include "baselib.h"
#include "EntityComponentBase.h"
#include "entity.h"

namespace Engine {
namespace Scene {
namespace Entity {

EntityComponentBase::EntityComponentBase(Entity &entity) :
    mEntity(entity)
{
}

EntityComponentBase::~EntityComponentBase() {

}

Entity &EntityComponentBase::getEntity() const
{
    return mEntity;
}


std::array<float, 2> EntityComponentBase::getPosition2D()
{
    return mEntity.getPosition2D();
}

void EntityComponentBase::setPosition(const std::array<float, 3> & pos)
{
	mEntity.setPosition(pos);
}

std::array<float, 3> EntityComponentBase::getPosition()
{
    return mEntity.getPosition();
}

std::array<float, 2> EntityComponentBase::getCenter2D()
{
	return mEntity.getCenter2D();
}

std::array<float, 3> EntityComponentBase::getCenter()
{
	return mEntity.getCenter();
}

std::array<float, 4> EntityComponentBase::getOrientation()
{
	return mEntity.getOrientation();
}

void EntityComponentBase::setScale(const std::array<float, 3> & scale)
{
	mEntity.setScale(scale);
}

std::array<float, 3> EntityComponentBase::getScale()
{
	return mEntity.getScale();
}

}
}
}
