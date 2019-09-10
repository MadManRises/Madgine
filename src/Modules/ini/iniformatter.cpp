#include "../moduleslib.h"

#include "iniformatter.h"

#include "../serialize/streams/serializestream.h"

namespace Engine {
namespace Ini {

    IniFormatter::IniFormatter()
        : Formatter(false, true)
    {
    }

    void IniFormatter::beginMember(Serialize::SerializeOutStream &out, const char *name, bool first)
    {
        out.writeUnformatted(std::string(name));
        out.writeUnformatted("="s);
    }

    void IniFormatter::endMember(Serialize::SerializeOutStream &out, const char *name, bool first)
    {
        out.writeUnformatted("\n"s);
    }

    void IniFormatter::beginMember(Serialize::SerializeInStream &in, const char *name, bool first)
    {
        std::string prefix = in.readN(strlen(name) + 1);
        if (prefix != std::string(name) + "=")
            throw 0;
    }

    void IniFormatter::endMember(Serialize::SerializeInStream &in, const char *name, bool first)
    {
        /*if (in.readPlainN(1) != "\n")
            throw 0;*/
    }

    void IniFormatter::writeEOL(Serialize::SerializeOutStream &)
    {
        throw "Not implemented";
    }

    bool IniFormatter::readEOL(Serialize::SerializeInStream &)
    {
        throw "Not implemented";
    }

    std::string IniFormatter::lookupFieldName(Serialize::SerializeInStream &in)
    {
        std::string name = in.peekUntil('=');
        if (!name.empty())
            name = name.substr(0, name.size() - 1);
        return name;
    }

}
}
