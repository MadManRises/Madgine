#include "../metalib.h"

#include "formatter.h"

#include "streams/streamerror.h"

namespace Engine {
namespace Serialize {

    StreamResult Formatter::beginExtended(Serialize::SerializeInStream &, const char *name, size_t count)
    {
        return StreamResult();
    }

	StreamResult Formatter::beginCompound(SerializeInStream &, const char *name)
    {
        return StreamResult();
    }

    StreamResult Formatter::endCompound(SerializeInStream &, const char *name)
    {
        return StreamResult();
    }

	StreamResult Formatter::beginPrimitive(SerializeInStream &, const char *name, uint8_t typeId)
    {
        return StreamResult();
    }

    StreamResult Formatter::endPrimitive(SerializeInStream &, const char *name, uint8_t typeId)
    {
        return StreamResult();
    }

    StreamResult Formatter::beginContainer(SerializeInStream &, const char *name, bool sized)
    {
        return StreamResult();
    }

    StreamResult Formatter::endContainer(SerializeInStream &, const char *name)
    {
        return StreamResult();
    }

}
}