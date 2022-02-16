#include "../metalib.h"

#include "unithelper.h"

#include "streams/serializestream.h"

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
            return STREAM_INTEGRITY_ERROR(in.stream(), in.isBinary(), "Unknown Serializable Unit-Id (" << id << ") used!");
        out = in.serializableList().at(id);
        return {};
    }
}
}
