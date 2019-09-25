#include "../moduleslib.h"

#include "iniformatter.h"

#include "../serialize/streams/serializestream.h"

namespace Engine {
namespace Ini {

    IniFormatter::IniFormatter()
        : Formatter(false, true)
    {
    }

    void IniFormatter::beginPrimitive(Serialize::SerializeOutStream &out, const char *name, size_t typeId)
    {
        out.writeUnformatted(std::string(name));
        out.writeUnformatted("="s);
    }

    void IniFormatter::endPrimitive(Serialize::SerializeOutStream &out, const char *name, size_t typeId)
    {
        out.writeUnformatted("\n"s);
    }

    void IniFormatter::beginPrimitive(Serialize::SerializeInStream &in, const char *name, size_t typeId)
    {
        std::string prefix = in.readN(strlen(name) + 1);
        if (prefix != std::string(name) + "=")
            throw 0;
    }

    void IniFormatter::endPrimitive(Serialize::SerializeInStream &in, const char *name, size_t typeId)
    {
        /*if (in.readPlainN(1) != "\n")
            throw 0;*/
    }

    std::string IniFormatter::lookupFieldName(Serialize::SerializeInStream &in)
    {
        std::string name = in.peekUntil("=");
        if (!name.empty())
            name = name.substr(0, name.size() - 1);
        return name;
    }

}
}
