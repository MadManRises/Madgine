#include "baselib.h"
#include "entitycomponentbase.h"
#include "entity.h"

namespace Engine {
namespace Scene {
namespace Entity {

EntityComponentBase::EntityComponentBase(Entity &entity, const Scripting::LuaTable &initTable) :
	SerializableUnitBase(entity.topLevel()),
    mEntity(entity),
	mInitTable(initTable)
{
}

EntityComponentBase::~EntityComponentBase() {

}

bool EntityComponentBase::init()
{
	return ScopeBase::init();
}

Entity &EntityComponentBase::getEntity() const
{
    return mEntity;
}


std::array<float, 2> EntityComponentBase::getPosition2D()
{
    return mEntity.getPosition2D();
}

void EntityComponentBase::setPosition(const Vector3 & pos)
{
	mEntity.setPosition(pos);
}

Vector3 EntityComponentBase::getPosition()
{
    return mEntity.getPosition();
}

std::array<float, 2> EntityComponentBase::getCenter2D()
{
	return mEntity.getCenter2D();
}

Vector3 EntityComponentBase::getCenter()
{
	return mEntity.getCenter();
}

std::array<float, 4> EntityComponentBase::getOrientation()
{
	return mEntity.getOrientation();
}

void EntityComponentBase::setScale(const Vector3 & scale)
{
	mEntity.setScale(scale);
}

Vector3 EntityComponentBase::getScale()
{
	return mEntity.getScale();
}

}
}
}
