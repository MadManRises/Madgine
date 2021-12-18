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
    }

    void IniFormatter::setupStream(std::ostream &out)
    {
        out << std::boolalpha;
    }

    void IniFormatter::beginPrimitive(SerializeOutStream &out, const char *name, uint8_t typeId)
    {
        out.writeUnformatted(std::string(name));
        out.writeUnformatted("=");
    }

    StreamResult IniFormatter::beginPrimitive(SerializeInStream &in, const char *name, uint8_t typeId)
    {

        std::string prefix = in.readUntil("=");
        if (name && StringUtil::substr(prefix, 0, -1) != name)
            return STREAM_PARSE_ERROR(in, "Expected '" << name << "'");

        if (typeId == Serialize::PrimitiveTypeIndex_v<std::string>)
            in.setNextFormattedStringDelimiter('\n');

        return {};
    }

    void IniFormatter::endPrimitive(SerializeOutStream &out, const char *name, uint8_t typeId)
    {
        out.writeUnformatted("\n");
    }

    StreamResult IniFormatter::endPrimitive(SerializeInStream &in, const char *name, uint8_t typeId)
    {
        return {};
    }

    std::string IniFormatter::lookupFieldName(SerializeInStream &in)
    {
        std::string name = in.peekUntil("=");
        if (!name.empty())
            name = StringUtil::substr(name, 0, -1);
        return name;
    }

    void IniFormatter::beginContainer(SerializeOutStream &out, const char *name, uint32_t size)
    {
        if (size != std::numeric_limits<uint32_t>::max()) {
            out.writeUnformatted("size=");
            out.writeUnformatted(size);
            out.writeUnformatted("\n");
        }
    }

    StreamResult IniFormatter::beginContainer(SerializeInStream &in, const char *name, bool sized)
    {
        uint32_t size = 0;
        if (sized) {
            STREAM_PROPAGATE_ERROR(in.format().beginExtended(in, name, 1));
            constexpr std::string_view prefix = "size=";
            if (in.readN(prefix.size()) != prefix)
                return STREAM_PARSE_ERROR(in, "Expected container size");
            STREAM_PROPAGATE_ERROR(in.readUnformatted(size));
        }
        mContainerSizes.push(size);
        return {};
    }

    StreamResult IniFormatter::endContainer(SerializeInStream &, const char *name)
    {
        assert(mContainerSizes.top() == 0);
        mContainerSizes.pop();
        return {};
    }

    bool IniFormatter::hasContainerItem(SerializeInStream &)
    {
        if (mContainerSizes.top() > 0) {
            --mContainerSizes.top();
            return true;
        }
        return false;
    }

}
}