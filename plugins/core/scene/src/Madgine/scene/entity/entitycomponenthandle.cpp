#include "../../scenelib.h"
#include "entitycomponenthandle.h"

#include "entitycomponenthandle.h"

#include "Meta/serialize/streams/operations.h"

#include "entity.h"

#include "../scenemanager.h"

#include "entitycomponentlistbase.h"

#include "entitycomponentptr.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        void entityComponentHelperWrite(Serialize::SerializeOutStream &out, const EntityComponentHandle<EntityComponentBase> &index, const char *name, const SceneManager *mgr)
        {
            static_assert(Serialize::has_function_writeState2_v<EntityComponentOwningHandle<EntityComponentBase>>);
            write(out, mgr->entityComponentList(index.mType).getEntity(index), name);
        }

        Serialize::StreamResult entityComponentHelperRead(Serialize::SerializeInStream &in, EntityComponentHandle<EntityComponentBase> &index, const char *name, SceneManager *mgr)
        {
            Entity *entity;
            STREAM_PROPAGATE_ERROR(read(in, entity, name));
            if (entity)
                index = entity->getComponent(index.mType).handle();
            return {};
        }

        void entityComponentOwningHelperWrite(Serialize::SerializeOutStream &out, const EntityComponentHandle<EntityComponentBase> &index, const char *name, CallerHierarchyBasePtr hierarchy)
        {
            const SceneManager *mgr = hierarchy;
            mgr->entityComponentList(index.mType).writeState(index, out, name, hierarchy);
        }

        Serialize::StreamResult entityComponentOwningHelperRead(Serialize::SerializeInStream &in, const EntityComponentHandle<EntityComponentBase> &index, const char *name, CallerHierarchyBasePtr hierarchy)
        {
            SceneManager *mgr = hierarchy;
            return mgr->entityComponentList(index.mType).readState(index, in, name, hierarchy);
        }

    }
}
}
