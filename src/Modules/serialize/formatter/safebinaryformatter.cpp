#include "../../moduleslib.h"

#include "safebinaryformatter.h"

#include "../streams/serializestream.h"

namespace Engine {
namespace Serialize {

    constexpr const size_t SERIALIZE_MAGIC_NUMBER = 0x12345678;

    SafeBinaryFormatter::SafeBinaryFormatter()
        : Formatter(true)
    {
    }

    void SafeBinaryFormatter::beginPrimitive(SerializeOutStream &out, const char *name, size_t typeId)
    {
        out.writeRaw<size_t>(SERIALIZE_MAGIC_NUMBER + typeId);
    }

    void SafeBinaryFormatter::beginPrimitive(SerializeInStream &in, const char *name, size_t typeId)
    {
        size_t type;
        in.readRaw(type);
        type -= SERIALIZE_MAGIC_NUMBER;
        if (type != typeId)
            throw SerializeException(Database::Exceptions::unknownSerializationType);
    }

}
}