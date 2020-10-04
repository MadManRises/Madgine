#include "../moduleslib.h"

#include "unithelper.h"

#include "serializable.h"

#include "streams/serializestream.h"

namespace Engine {
namespace Serialize {    

	SyncableUnitBase *Engine::Serialize::convertPtr(SerializeInStream &in, UnitId id)
    {
        return in.convertPtr(id);
    }
}
}
