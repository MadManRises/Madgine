#include "../moduleslib.h"

#include "iniformatter.h"

#include "../serialize/streams/serializestream.h"

namespace Engine {
namespace Ini {



    IniFormatter::IniFormatter() : Formatter(true)
    {
    }

    void IniFormatter::beginMember(Serialize::SerializeOutStream &out, const char *name, bool first)
    {
        out.writePlain(std::string(name), *this);
        out.writePlain("="s, *this);
    }

    void IniFormatter::endMember(Serialize::SerializeOutStream &out, const char *name, bool first)
    {
        out.writePlain("\n"s, *this);
    }

    void IniFormatter::beginMember(Serialize::SerializeInStream &in, const char *name, bool first)
    {
        std::string prefix = in.readPlainN(strlen(name) + 1);
        if (prefix != std::string(name) + "=")
            throw 0;
    }

    void IniFormatter::endMember(Serialize::SerializeInStream &in, const char *name, bool first)
    {
        /*if (in.readPlainN(1) != "\n")
            throw 0;*/
    }

    std::string IniFormatter::lookupFieldName(Serialize::SerializeInStream &in)
    {
        std::string name = in.peekPlainUntil('=');
        if (!name.empty())
			name = name.substr(0, name.size() - 1);
        return name;
    }


}
}
