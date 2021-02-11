#include "../metalib.h"

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
    
    SerializableUnitBase *convertSerializablePtr(SerializeInStream &in, uint32_t id)
    {
        return in.serializableList().at(id);        
    }
}
}
