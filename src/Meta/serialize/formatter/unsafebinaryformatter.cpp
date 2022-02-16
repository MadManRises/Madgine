#include "../../metalib.h"

#include "unsafebinaryformatter.h"

#include "../streams/serializestream.h"

namespace Engine {
namespace Serialize {

    UnsafeBinaryFormatter::UnsafeBinaryFormatter()
        : Formatter(true)
    {
    }

    void UnsafeBinaryFormatter::beginContainerWrite(const char *name, uint32_t size)
    {
        if (size != std::numeric_limits<uint32_t>::max())
            mStream.write(size);
    }

    StreamResult UnsafeBinaryFormatter::beginContainerRead(const char *name, bool sized)
    {
        uint32_t size = 0;
        if (sized) {
            STREAM_PROPAGATE_ERROR(beginExtendedRead(name, 1));
            STREAM_PROPAGATE_ERROR(mStream.read(size));
        }
        mContainerSizes.push(size);
        return {};
    }

    StreamResult UnsafeBinaryFormatter::endContainerRead(const char *name)
    {
        assert(mContainerSizes.top() == 0);
        mContainerSizes.pop();
        return {};
    }

    bool UnsafeBinaryFormatter::hasContainerItem()
    {
        if (mContainerSizes.top() > 0) {
            --mContainerSizes.top();
            return true;
        }
        return false;
    }

}
}