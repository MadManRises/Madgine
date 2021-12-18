#include "../metalib.h"

#include "unithelper.h"

#include "streams/serializestream.h"

#include "serializemanager.h"

namespace Engine {
namespace Serialize {

    StreamResult convertSyncablePtr(SerializeInStream &in, UnitId id, SyncableUnitBase *&out)
    {
        return SerializeManager::convertPtr(in, id, out);
    }

    StreamResult convertSerializablePtr(SerializeInStream &in, uint32_t id, SerializableDataUnit *&out)
    {
        if (id > in.serializableList().size())
            return STREAM_INTEGRITY_ERROR(in, "Unknown Serializable Unit-Id (" << id << ") used!");
        out = in.serializableList().at(id);
        return {};
    }
}
}
