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

    void XMLFormatter::beginExtended(Serialize::SerializeOutStream &out, const char *name, size_t count)
    {
        if (!mCurrentExtended) {
            if (!name)
                name = "Item";
            out.writeUnformatted(indent() + "<" + std::string(name));
            mCurrentExtended = true;
        }
        assert(mCurrentExtendedCount == 0);
        assert(count > 0);
        mCurrentExtendedCount = count;
    }

    void XMLFormatter::beginCompound(Serialize::SerializeOutStream &out, const char *name)
    {
        if (!mCurrentExtended) {
            if (!name)
                name = "Item";
            out.writeUnformatted(indent() + "<" + std::string(name));
        } else {
            assert(mCurrentExtendedCount == 0);
        }
        out.writeUnformatted(">\n");
        mCurrentExtended = false;
        ++mLevel;
    }

    void XMLFormatter::endCompound(Serialize::SerializeOutStream &out, const char *name)
    {
        --mLevel;
        if (!name)
            name = "Item";
        out.writeUnformatted(indent() + "</" + std::string(name) + ">\n");
    }

    void XMLFormatter::beginExtended(Serialize::SerializeInStream &in, const char *name, size_t count)
    {
        if (!mCurrentExtended) {
            if (name) {
                std::string prefix = in.readN(strlen(name) + 2);
                if (!StringUtil::startsWith(prefix, "<" + std::string(name)))
                    std::terminate();
            } else {
                std::string prefix = in.readUntil(" ");
                if (!StringUtil::startsWith(prefix, "<"))
                    std::terminate();
                if (prefix.size() <= 1)
                    std::terminate();
            }
            mCurrentExtended = true;
        }
        assert(mCurrentExtendedCount == 0);
        assert(count > 0);
        mCurrentExtendedCount = count;
    }

    void XMLFormatter::beginCompound(Serialize::SerializeInStream &in, const char *name)
    {
        if (!mCurrentExtended) {
            if (name) {
                std::string prefix = in.readN(strlen(name) + 1);
                if (prefix != "<" + std::string(name))
                    std::terminate();
            } else {
                std::string prefix = in.peekUntil(">");
                if (!StringUtil::startsWith(prefix, "<"))
                    std::terminate();
                if (prefix.size() <= 1)
                    std::terminate();
                in.readN(prefix.size() - 1);
            }
        } else {
            assert(mCurrentExtendedCount == 0);
        }
        std::string prefix = in.readN(1);
        if (prefix != ">")
            std::terminate();
        mCurrentExtended = false;
    }

    void XMLFormatter::endCompound(Serialize::SerializeInStream &in, const char *name)
    {
        if (name) {
            std::string prefix = in.readN(strlen(name) + 3);
            if (prefix != "</" + std::string(name) + ">")
                std::terminate();
        } else {
            std::string prefix = in.readUntil(">");
            if (!StringUtil::startsWith(prefix, "</"))
                std::terminate();
            if (prefix.size() <= 2)
                std::terminate();
        }
    }

    void XMLFormatter::beginPrimitive(Serialize::SerializeOutStream &out, const char *name, uint8_t typeId)
    {
        if (mCurrentExtendedCount > 0) {
            assert(mCurrentExtended);
            --mCurrentExtendedCount;
            out.writeUnformatted(" " + std::string(name) + "=");
            if (typeId == Serialize::PrimitiveTypeIndex_v<std::string> || typeId == Serialize::PrimitiveTypeIndex_v<std::string_view> || typeId == Serialize::PrimitiveTypeIndex_v<Filesystem::Path>)
                out.writeUnformatted("\"");
        } else {
            if (!mCurrentExtended) {
                out.writeUnformatted(indent() + "<" + std::string(name));
            } else {
                mCurrentExtended = false;
            }
            out.writeUnformatted(">");
        }
    }

    void XMLFormatter::endPrimitive(Serialize::SerializeOutStream &out, const char *name, uint8_t typeId)
    {
        if (mCurrentExtended) {
            if (typeId == Serialize::PrimitiveTypeIndex_v<std::string> || typeId == Serialize::PrimitiveTypeIndex_v<std::string_view> || typeId == Serialize::PrimitiveTypeIndex_v<Filesystem::Path>)
                out.writeUnformatted("\"");
        } else
            out.writeUnformatted("</" + std::string(name) + ">\n");
    }

    void XMLFormatter::beginPrimitive(Serialize::SerializeInStream &in, const char *name, uint8_t typeId)
    {
        if (mCurrentExtendedCount > 0) {
            assert(mCurrentExtended);
            --mCurrentExtendedCount;
            if (name) {
                std::string prefix = in.readN(strlen(name) + 1);
                if (prefix != std::string(name) + "=")
                    std::terminate();
            } else {
                if (in.readUntil("=").size() <= 1)
                    std::terminate();
            }
            if (typeId == Serialize::PrimitiveTypeIndex_v<std::string> || typeId == Serialize::PrimitiveTypeIndex_v<Filesystem::Path>) {
                if (in.readN(1) != "\"")
                    std::terminate();
                in.setNextFormattedStringDelimiter('"');
            }
        } else {
            if (!mCurrentExtended) {
                if (name) {
                    std::string prefix = in.readN(strlen(name) + 1);
                    if (prefix != "<" + std::string(name))
                        std::terminate();
                } else {
                    std::string prefix = in.peekUntil(">");
                    if (prefix.size() <= 1)
                        std::terminate();
                    in.readN(prefix.size() - 1);
                }
            } else {                
                mCurrentExtended = false;
            }
            std::string prefix = in.readN(1);
            if (prefix != ">")
                std::terminate();

            if (typeId == Serialize::PrimitiveTypeIndex_v<std::string> || typeId == Serialize::PrimitiveTypeIndex_v<Filesystem::Path>) {
                in.setNextFormattedStringDelimiter('<');
            }
        }
    }

    void XMLFormatter::endPrimitive(Serialize::SerializeInStream &in, const char *name, uint8_t typeId)
    {
        if (!mCurrentExtended) {
            const char *cPrefix = ((typeId == Serialize::PrimitiveTypeIndex_v<std::string> || typeId == Serialize::PrimitiveTypeIndex_v<Filesystem::Path>) ? "/" : "</");
            if (name) {
                std::string prefix = in.readN(strlen(name) + 1 + strlen(cPrefix));
                if (prefix != cPrefix + std::string(name) + ">")
                    std::terminate();
            } else {
                std::string prefix = in.readUntil(">");
                if (!StringUtil::startsWith(prefix, cPrefix))
                    std::terminate();
                if (prefix.size() <= 1)
                    std::terminate();
            }
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
