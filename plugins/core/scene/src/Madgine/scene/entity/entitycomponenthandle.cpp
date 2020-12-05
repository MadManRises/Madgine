#include "../../scenelib.h"
#include "entitycomponenthandle.h"

#include "entitycomponenthandle.h"

#include "Modules/serialize/streams/operations.h"

#include "entity.h"

#include "../scenemanager.h"

namespace Engine {
namespace Scene {

    void Entity::entityComponentHelperWrite(Serialize::SerializeOutStream &out, GenerationContainerIndex &index, const char *name, SceneManager *mgr, size_t componentIndex)
    {        
        write(out, mgr->toEntityPtr(mgr->entityComponentList(componentIndex).getEntity(index, mgr)).get(), name);
    }

    void Entity::entityComponentHelperRead(Serialize::SerializeInStream &in, GenerationContainerIndex &index, const char *name, SceneManager *mgr, size_t componentIndex)
    {
        Entity *entity;
        read(in, entity, name);
        EntityComponentPtr<EntityComponentBase> ptr = mgr->toEntityPtr(entity).getComponent(index);
        index = static_cast<EntityComponentHandle<EntityComponentBase>>(ptr).mIndex;
    }

}
}
