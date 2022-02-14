#include "Meta/metalib.h"

#include "iniformatter.h"

#include "../streams/serializestream.h"

#include "../primitivetypes.h"

namespace Engine {
namespace Serialize {

    IniFormatter::IniFormatter()
        : Formatter(false)
    {
    }

    void IniFormatter::setupStream(std::istream &in)
    {
        in >> std::boolalpha;
        in >> std::noskipws;
    }

    void IniFormatter::setupStream(std::ostream &out)
    {
        out << std::boolalpha;
    }

    void IniFormatter::beginPrimitiveWrite(const char *name, uint8_t typeId)
    {
        mOut << name << "=";
    }

    StreamResult IniFormatter::beginPrimitiveRead(const char *name, uint8_t typeId)
    {
        mIn.skipWs(true);
        std::string prefix;
        STREAM_PROPAGATE_ERROR(mIn.readUntil(prefix, "="));
        if (name && StringUtil::substr(prefix, 0, -1) != name)
            return STREAM_PARSE_ERROR(mIn, mBinary, "Expected '" << name << "', Got '" << StringUtil::substr(prefix, 0, -1) << "'");

        if (typeId == Serialize::PrimitiveTypeIndex_v<std::string>)
            mNextStringDelimiter = "\n";

        return {};
    }

    void IniFormatter::endPrimitiveWrite(const char *name, uint8_t typeId)
    {
        mOut << "\n";
    }

    StreamResult IniFormatter::endPrimitiveRead(const char *name, uint8_t typeId)
    {
        return {};
    }

    StreamResult IniFormatter::lookupFieldName(std::string &name)
    {
        STREAM_PROPAGATE_ERROR(mIn.peekUntil(name, "="));
        if (!name.empty())
            name = StringUtil::substr(name, 0, -1);
        return {};
    }

    void IniFormatter::beginContainerWrite(const char *name, uint32_t size)
    {
        if (size != std::numeric_limits<uint32_t>::max()) {
            mOut << "size=" << size << "\n";
        }
    }

    StreamResult IniFormatter::beginContainerRead(const char *name, bool sized)
    {
        uint32_t size = 0;
        if (sized) {
            mIn.skipWs(true);
            FORMATTER_EXPECT("size=");
            STREAM_PROPAGATE_ERROR(read(size));
        }
        mContainerSizes.push(size);
        return {};
    }

    StreamResult IniFormatter::endContainerRead(const char *name)
    {
        assert(mContainerSizes.top() == 0);
        mContainerSizes.pop();
        return {};
    }

    bool IniFormatter::hasContainerItem()
    {
        if (mContainerSizes.top() > 0) {
            --mContainerSizes.top();
            return true;
        }
        return false;
    }

}
}