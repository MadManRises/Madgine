#include "../../scenelib.h"

#include "entitycomponentlistbase.h"

#include "Meta/serialize/operations.h"

namespace Engine {
namespace Scene {
    namespace Entity {
        
        Serialize::StreamResult EntityComponentListBase::readState(const EntityComponentHandle<EntityComponentBase> &index, Serialize::FormattedSerializeStream &in, const char *name, CallerHierarchyBasePtr hierarchy)
        {
            Serialize::SerializableDataPtr ptr = getSerialized(index);
            return Serialize::read(in, ptr, name, hierarchy);
        }

        void EntityComponentListBase::writeState(const EntityComponentHandle<EntityComponentBase> &index, Serialize::FormattedSerializeStream &out, const char *name, CallerHierarchyBasePtr hierarchy) const
        {
            Serialize::write(out, getSerialized(index), name, hierarchy);
        }

    }
}
}