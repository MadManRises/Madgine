#include "../../scenelib.h"
#include "entitycomponenthandle.h"

#include "entitycomponenthandle.h"

#include "Modules/serialize/streams/operations.h"

#include "entity.h"

#include "../scenemanager.h"

#include "entitycomponentlistbase.h"

#include "entitycomponentptr.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        void entityComponentHelperWrite(Serialize::SerializeOutStream &out, const EntityComponentHandle<EntityComponentBase> &index, const char *name, SceneManager *mgr)
        {
            write(out, mgr->entityComponentList(index.mType).getEntity(index), name);
        }

        void entityComponentHelperRead(Serialize::SerializeInStream &in, EntityComponentHandle<EntityComponentBase> &index, const char *name, SceneManager *mgr)
        {
            Entity *entity;
            read(in, entity, name);
            index = entity->getComponent(index.mType).handle();
        }

    }
}
}
