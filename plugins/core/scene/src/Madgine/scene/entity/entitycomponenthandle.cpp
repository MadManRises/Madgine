#include "../../scenelib.h"
#include "entitycomponenthandle.h"

#include "Meta/serialize/operations.h"

#include "entity.h"

#include "../scenemanager.h"

#include "entitycomponentlistbase.h"

#include "entitycomponentptr.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        void entityComponentHelperWrite(Serialize::FormattedSerializeStream &out, const EntityComponentHandle<EntityComponentBase> &index, const char *name, const SceneManager *mgr)
        {
            write(out, mgr->entityComponentList(index.mType).getEntity(index), name);
        }

        Serialize::StreamResult entityComponentHelperRead(Serialize::FormattedSerializeStream &in, EntityComponentHandle<EntityComponentBase> &index, const char *name, SceneManager *mgr)
        {
            Entity *entity;
            STREAM_PROPAGATE_ERROR(read(in, entity, name));
            if (entity)
                index = entity->getComponent(index.mType).handle();
            return {};
        }

        void entityComponentOwningHelperWrite(Serialize::FormattedSerializeStream &out, const EntityComponentHandle<EntityComponentBase> &index, const char *name, CallerHierarchyBasePtr hierarchy)
        {
            const SceneManager *mgr = hierarchy;
            mgr->entityComponentList(index.mType).writeState(index, out, name, hierarchy);
        }

        Serialize::StreamResult entityComponentOwningHelperRead(Serialize::FormattedSerializeStream &in, const EntityComponentHandle<EntityComponentBase> &index, const char *name, CallerHierarchyBasePtr hierarchy)
        {
            SceneManager *mgr = hierarchy;
            return mgr->entityComponentList(index.mType).readState(index, in, name, hierarchy);
        }

        Serialize::StreamResult entityComponentOwningHelperApplyMap(Serialize::FormattedSerializeStream &in, EntityComponentHandle<EntityComponentBase> &index, bool success, CallerHierarchyBasePtr hierarchy)
        {
            SceneManager *mgr = hierarchy;
            return mgr->entityComponentList(index.mType).applyMap(index, in, success, hierarchy);
        }

    }
}
}
