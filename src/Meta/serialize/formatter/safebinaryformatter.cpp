#include "../../metalib.h"

#include "safebinaryformatter.h"

#include "../streams/serializestream.h"

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

    StreamResult SafeBinaryFormatter::beginPrimitive(SerializeInStream &in, const char *name, uint8_t typeId)
    {
        uint16_t type;
        in.readRaw(type);
        type -= SERIALIZE_MAGIC_NUMBER;
        if (type != typeId)
            return STREAM_PARSE_ERROR(in, "Invalid type-id encountered!");
        return {};
    }

    void SafeBinaryFormatter::beginContainer(SerializeOutStream &out, const char *name, uint32_t size)
    {
        if (size != std::numeric_limits<uint32_t>::max())
            out.writeRaw(size);        
    }

    StreamResult SafeBinaryFormatter::beginContainer(SerializeInStream &in, const char *name, bool sized)
    {
        uint32_t size = 0;
        if (sized) {
            STREAM_PROPAGATE_ERROR(in.format().beginExtended(in, name, 1));
            STREAM_PROPAGATE_ERROR(in.readRaw(size));
        } 
        mContainerSizes.push(size);
        return {};
    }

    StreamResult SafeBinaryFormatter::endContainer(SerializeInStream &, const char *name)
    {
        assert(mContainerSizes.top() == 0);
        mContainerSizes.pop();
        return {};
    }

    bool SafeBinaryFormatter::hasContainerItem(SerializeInStream &)
    {
        if (mContainerSizes.top() > 0) {
            --mContainerSizes.top();
            return true;
        }
        return false;
    }

}
}