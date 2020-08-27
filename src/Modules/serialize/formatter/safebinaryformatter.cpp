#include "../../moduleslib.h"

#include "safebinaryformatter.h"

#include "../streams/serializestream.h"

#include "../serializeexception.h"

namespace Engine {
namespace Serialize {

    constexpr const uint16_t SERIALIZE_MAGIC_NUMBER = 0x5AFE;

    SafeBinaryFormatter::SafeBinaryFormatter()
        : Formatter(true)
    {
    }

    void SafeBinaryFormatter::beginPrimitive(SerializeOutStream &out, const char *name, uint8_t typeId)
    {
        out.writeRaw<uint16_t>(SERIALIZE_MAGIC_NUMBER + typeId);
    }

    void SafeBinaryFormatter::beginPrimitive(SerializeInStream &in, const char *name, uint8_t typeId)
    {
        uint16_t type;
        in.readRaw(type);
        type -= SERIALIZE_MAGIC_NUMBER;
        if (type != typeId)
            throw SerializeException("Unknown Serialization Type");
    }

}
}