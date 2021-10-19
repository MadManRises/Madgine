#include "../metalib.h"

#include "unithelper.h"

#include "streams/serializestream.h"

#include "serializemanager.h"

namespace Engine {
namespace Serialize {    

	SyncableUnitBase *convertSyncablePtr(SerializeInStream &in, UnitId id)
    {
        return in.manager()->convertPtr(in, id);
    }
    
    SerializableDataUnit *convertSerializablePtr(SerializeInStream &in, uint32_t id)
    {
        return in.serializableList().at(id);        
    }
}
}
