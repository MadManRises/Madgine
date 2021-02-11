#include "../../scenelib.h"

#include "entitycomponentlistbase.h"

#include "Meta/serialize/streams/operations.h"

namespace Engine {
namespace Scene {
    namespace Entity {
        
        void EntityComponentListBase::readState(const EntityComponentHandle<EntityComponentBase> &index, Serialize::SerializeInStream &in, const char *name, CallerHierarchyBasePtr hierarchy)
        {
            Serialize::SerializableDataPtr ptr = getSerialized(index);
            Serialize::read(in, ptr, name, hierarchy);
        }

        void EntityComponentListBase::writeState(const EntityComponentHandle<EntityComponentBase> &index, Serialize::SerializeOutStream &out, const char *name, CallerHierarchyBasePtr hierarchy) const
        {
            Serialize::write(out, getSerialized(index), name, hierarchy);
        }

    }
}
}