#include "baselib.h"
#include "entitycomponentbase.h"
#include "entity.h"

namespace Engine {
namespace Scene {
namespace Entity {

EntityComponentBase::EntityComponentBase(Entity &entity, const Scripting::LuaTable &initTable) :
    mEntity(entity),
	mInitTable(initTable)
{
}

EntityComponentBase::~EntityComponentBase() {

}

void EntityComponentBase::init()
{
	
}

void EntityComponentBase::finalize()
{
}

Entity &EntityComponentBase::getEntity() const
{
    return mEntity;
}


}
}
}
