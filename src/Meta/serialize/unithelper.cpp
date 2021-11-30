#include "../metalib.h"

#include "unithelper.h"

#include "streams/serializestream.h"

#include "serializemanager.h"

namespace Engine {
namespace Serialize {

    StreamResult convertSyncablePtr(SerializeInStream &in, UnitId id, SyncableUnitBase *&out)
    {
        return in.manager()->convertPtr(in, id, out);
    }

    StreamResult convertSerializablePtr(SerializeInStream &in, uint32_t id, SerializableDataUnit *&out)
    {
        if (id > in.serializableList().size())
            throw 0;
        out = in.serializableList().at(id);
        return {};
    }
}
}
