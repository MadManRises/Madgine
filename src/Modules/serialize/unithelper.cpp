#include "../moduleslib.h"

#include "unithelper.h"

#include "serializable.h"

#include "streams/serializestream.h"

#include "serializemanager.h"

namespace Engine {
namespace Serialize {    

	SyncableUnitBase *convertSyncablePtr(SerializeInStream &in, UnitId id)
    {
        return in.manager()->convertPtr(in, id);
    }
    
    SerializableUnitBase *convertSerializablePtr(SerializeInStream &in, uint64_t id)
    {
        return in.serializableMap().at(id);        
    }
}
}
