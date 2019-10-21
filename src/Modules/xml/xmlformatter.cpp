#include "../moduleslib.h"

#include "xmlformatter.h"

#include "../serialize/streams/serializestream.h"

#include "Interfaces/stringutil.h"

namespace Engine {
namespace XML {

    XMLFormatter::XMLFormatter()
        : Formatter(false, true)
    {
    }

    void XMLFormatter::setupStream(std::istream &in)
    {
    }

    void XMLFormatter::setupStream(std::ostream &out)
    {
    }

    void XMLFormatter::beginExtendedCompound(Serialize::SerializeOutStream &out, const char *name)
    {
        if (!mCurrentExtended) {
            out.writeUnformatted(indent() + "<" + std::string(name));
            mCurrentExtended = true;
        }
    }

    void XMLFormatter::beginCompound(Serialize::SerializeOutStream &out, const char *name)
    {
        if (!mCurrentExtended) {
            out.writeUnformatted(indent() + "<" + std::string(name));
        }
        out.writeUnformatted(">\n");
        mCurrentExtended = false;
        ++mLevel;
    }

    void XMLFormatter::endCompound(Serialize::SerializeOutStream &out, const char *name)
    {
        --mLevel;
        out.writeUnformatted(indent() + "</" + std::string(name) + ">\n");
    }

    void XMLFormatter::beginExtendedCompound(Serialize::SerializeInStream &in, const char *name)
    {
        if (!mCurrentExtended) {
            std::string prefix = in.readN(strlen(name) + 2);
            if (!StringUtil::startsWith(prefix, "<" + std::string(name)))
                std::terminate();
            mCurrentExtended = true;
        }
    }

    void XMLFormatter::beginCompound(Serialize::SerializeInStream &in, const char *name)
    {
        if (!mCurrentExtended) {
            std::string prefix = in.readN(strlen(name) + 1);
            if (prefix != "<" + std::string(name))
                std::terminate();
        }
        std::string prefix = in.readN(1);
        if (prefix != ">")
            std::terminate();
        mCurrentExtended = false;
    }

    void XMLFormatter::endCompound(Serialize::SerializeInStream &in, const char *name)
    {
        std::string prefix = in.readN(strlen(name) + 3);
        if (prefix != "</" + std::string(name) + ">")
            std::terminate();
    }

    void XMLFormatter::beginPrimitive(Serialize::SerializeOutStream &out, const char *name, size_t typeId)
    {
        if (mCurrentExtended) {
            out.writeUnformatted(" " + std::string(name) + "=");
            if (typeId == Serialize::PrimitiveTypeIndex_v<std::string> || typeId == Serialize::PrimitiveTypeIndex_v<Filesystem::Path>)
                out.writeUnformatted("\"");
        } else
            out.writeUnformatted(indent() + "<" + std::string(name) + ">");
    }

    void XMLFormatter::endPrimitive(Serialize::SerializeOutStream &out, const char *name, size_t typeId)
    {
        if (mCurrentExtended) {
            if (typeId == Serialize::PrimitiveTypeIndex_v<std::string> || typeId == Serialize::PrimitiveTypeIndex_v<Filesystem::Path>)
                out.writeUnformatted("\"");
        } else
            out.writeUnformatted("</" + std::string(name) + ">\n");
    }

    void XMLFormatter::beginPrimitive(Serialize::SerializeInStream &in, const char *name, size_t typeId)
    {
        if (mCurrentExtended) {
            if (name) {
                std::string prefix = in.readN(strlen(name) + 1);
                if (prefix != std::string(name) + "=")
                    std::terminate();
            } else {
                if (in.readUntil("=").empty())
                    std::terminate();
            }
            if (typeId == Serialize::PrimitiveTypeIndex_v<std::string> || typeId == Serialize::PrimitiveTypeIndex_v<Filesystem::Path>) {
                if (in.readN(1) != "\"")
                    std::terminate();
                in.setNextFormattedStringDelimiter('"');
            }
        } else {
            std::string prefix = in.readN(strlen(name) + 2);
            if (prefix != "<" + std::string(name) + ">")
                std::terminate();
            if (typeId == Serialize::PrimitiveTypeIndex_v<std::string> || typeId == Serialize::PrimitiveTypeIndex_v<Filesystem::Path>) {
                in.setNextFormattedStringDelimiter('<');
            }
        }
    }

    void XMLFormatter::endPrimitive(Serialize::SerializeInStream &in, const char *name, size_t typeId)
    {
        if (!mCurrentExtended) {
            const char *cPrefix = ((typeId == Serialize::PrimitiveTypeIndex_v<std::string> || typeId == Serialize::PrimitiveTypeIndex_v<Filesystem::Path>) ? "/" : "</");
            std::string prefix = in.readN(strlen(name) + 1 + strlen(cPrefix));
            if (prefix != cPrefix + std::string(name) + ">")
                std::terminate();
        }
    }

    std::string XMLFormatter::lookupFieldName(Serialize::SerializeInStream &in)
    {
        std::string name = in.peekUntil("> ");
        if (!name.empty()) {
            if (name[0] != '<')
                std::terminate();
            if (name[1] == '/')
                return {};
            return name.substr(1, name.size() - 2);
        }
        return name;
    }

    std::string XMLFormatter::indent()
    {
        return std::string(4 * mLevel, ' ');
    }

}
}
