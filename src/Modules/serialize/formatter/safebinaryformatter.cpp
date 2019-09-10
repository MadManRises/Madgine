#include "../../moduleslib.h"

#include "safebinaryformatter.h"

#include "../streams/serializestream.h"

namespace Engine {
namespace Serialize {

    constexpr const size_t SERIALIZE_MAGIC_NUMBER = 0x12345678;
    constexpr const size_t SERIALIZE_EOL_MAGIC_NUMBER = 0x13579BDF;

    SafeBinaryFormatter::SafeBinaryFormatter()
        : Formatter(true)
    {
    }

    void SafeBinaryFormatter::beginPrimitive(SerializeOutStream &out, size_t typeId)
    {
        out.writeRaw<size_t>(SERIALIZE_MAGIC_NUMBER + typeId);
    }

    void SafeBinaryFormatter::beginPrimitive(SerializeInStream &in, size_t typeId)
    {
        size_t type;
        in.readRaw(type);
        if (type != SERIALIZE_MAGIC_NUMBER + typeId)
            throw SerializeException(Database::Exceptions::unknownSerializationType);
    }

    void SafeBinaryFormatter::writeEOL(SerializeOutStream &out)
    {
        out.writeRaw<size_t>(SERIALIZE_EOL_MAGIC_NUMBER);
    }

    bool SafeBinaryFormatter::readEOL(SerializeInStream &in)
    {
        pos_type pos = in.tell();
        size_t type;
        in.readRaw(type);
        if (type == SERIALIZE_EOL_MAGIC_NUMBER) {
            return true;
        }
        in.seek(pos);
        return false;
    }

}
}