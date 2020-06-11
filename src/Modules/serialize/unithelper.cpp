#include "../moduleslib.h"

#include "unithelper.h"

#include "serializable.h"
#include "toplevelserializableunit.h"

#include "streams/serializestream.h"

namespace Engine {
namespace Serialize {    

	SerializableUnitBase *Engine::Serialize::convertPtr(SerializeInStream &in, size_t id)
    {
        return in.convertPtr(id);
    }
}
}
