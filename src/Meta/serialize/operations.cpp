#include "../metalib.h"

#include "operations.h"

#include "serializemanager.h"
#include "streams/formattedserializestream.h"

namespace Engine {
namespace Serialize {

    StreamResult convertSyncablePtr(FormattedSerializeStream &in, UnitId id, SyncableUnitBase *&out)
    {
        return SerializeManager::convertPtr(in.stream(), id, out);
    }

    StreamResult convertSerializablePtr(FormattedSerializeStream &in, uint32_t id, SerializableDataUnit *&out)
    {
        if (id > in.serializableList().size())
            return STREAM_INTEGRITY_ERROR(in) << "Unknown Serializable Unit-Id (" << id << ") used!";
        out = in.serializableList().at(id);
        return {};
    }
}
}
