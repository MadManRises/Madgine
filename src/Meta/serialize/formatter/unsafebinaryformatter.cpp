#include "../../metalib.h"

#include "unsafebinaryformatter.h"

#include "../streams/serializestream.h"

namespace Engine {
namespace Serialize {

    UnsafeBinaryFormatter::UnsafeBinaryFormatter()
        : Formatter(true)
    {
    }

    void UnsafeBinaryFormatter::beginContainer(SerializeOutStream &out, const char *name, uint32_t size)
    {
        if (size != std::numeric_limits<uint32_t>::max())
            out.writeRaw(size);
    }

    StreamResult UnsafeBinaryFormatter::beginContainer(SerializeInStream &in, const char *name, bool sized)
    {
        uint32_t size = 0;
        if (sized) {
            STREAM_PROPAGATE_ERROR(in.format().beginExtended(in, name, 1));
            STREAM_PROPAGATE_ERROR(in.readRaw(size));
        }
        mContainerSizes.push(size);
        return {};
    }

    StreamResult UnsafeBinaryFormatter::endContainer(SerializeInStream &, const char *name)
    {
        assert(mContainerSizes.top() == 0);
        mContainerSizes.pop();
        return {};
    }

    bool UnsafeBinaryFormatter::hasContainerItem(SerializeInStream &)
    {
        if (mContainerSizes.top() > 0) {
            --mContainerSizes.top();
            return true;
        }
        return false;
    }

}
}