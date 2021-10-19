#include "Meta/metalib.h"

#include "xmlformatter.h"

#include "../streams/serializestream.h"

#include "../primitivetypes.h"

namespace Engine {
namespace Serialize {

    XMLFormatter::XMLFormatter()
        : Formatter(false, true)
    {
    }

    void XMLFormatter::beginExtended(SerializeOutStream &out, const char *name, size_t count)
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

    StreamResult XMLFormatter::beginExtended(SerializeInStream &in, const char *name, size_t count)
    {
        if (!mCurrentExtended) {
            if (name) {
                std::string prefix = in.readN(strlen(name) + 1);
                if (prefix != "<" + std::string(name))
                    return STREAM_PARSE_ERROR(in, "Expected extended opening tag '<" << name << "'");
            } else {
                std::string prefix = in.readUntil(" ");
                if (!StringUtil::startsWith(prefix, "<"))
                    return STREAM_PARSE_ERROR(in, "Expected extended opening tag, found: " << prefix);
                if (prefix.size() <= 1)
                    return STREAM_PARSE_ERROR(in, "Expected extended opening tag");
            }
            mCurrentExtended = true;
        }
        assert(mCurrentExtendedCount == 0);
        assert(count > 0);
        mCurrentExtendedCount = count;
        return {};
    }

    void XMLFormatter::beginCompound(SerializeOutStream &out, const char *name)
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

    StreamResult XMLFormatter::beginCompound(SerializeInStream &in, const char *name)
    {
        if (!mCurrentExtended) {
            std::string prefix = in.readUntil(">");
            if (name && prefix != "<" + std::string(name) + ">")
                return STREAM_PARSE_ERROR(in, "Expected opening tag <" << name << ">, found: " << prefix);
            in.seek(in.tell() - off_type { 1 });
        } else {
            assert(mCurrentExtendedCount == 0);
            STREAM_PROPAGATE_ERROR(prefetchAttributes(in));
            std::string next = in.peekN(1);
            if (next != ">")
                return STREAM_PARSE_ERROR(in, "Expected '>'");            
        }
        std::string prefix = in.readN(1);
        if (prefix != ">")
            return STREAM_PARSE_ERROR(in, "Expected closed tag '>'");
        mCurrentExtended = false;
        return {};
    }
    void XMLFormatter::endCompound(SerializeOutStream &out, const char *name)
    {
        --mLevel;
        if (!name)
            name = "Item";
        out.writeUnformatted(indent() + "</" + std::string(name) + ">\n");
    }

    StreamResult XMLFormatter::endCompound(SerializeInStream &in, const char *name)
    {
        std::string prefix = in.readUntil(">");
        if (name && prefix != "</" + std::string(name) + ">")
            return STREAM_PARSE_ERROR(in, "Expected closing tag '</" << name << ">', found: " << prefix);
        if (!StringUtil::startsWith(prefix, "</"))
            return STREAM_PARSE_ERROR(in, "Expected closing tag");
        return {};
    }

    void XMLFormatter::beginPrimitive(SerializeOutStream &out, const char *name, uint8_t typeId)
    {
        if (!name)
            name = "Element";
        if (mCurrentExtendedCount > 0) {
            assert(mCurrentExtended);
            --mCurrentExtendedCount;
            out.writeUnformatted(" " + std::string(name) + "=");
            if (typeId == Serialize::PrimitiveTypeIndex_v<std::string>)
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

    StreamResult XMLFormatter::beginPrimitive(SerializeInStream &in, const char *name, uint8_t typeId)
    {
        if (mCurrentExtendedCount > 0) {
            assert(mCurrentExtended);
            --mCurrentExtendedCount;
            auto it = mPrefetchedAttributes.find(name);
            if (it != mPrefetchedAttributes.end()) {
                mExtendedLookupPos = in.tell();
                in.seek(it->second);
                mPrefetchedAttributes.erase(it);
            } else {
                STREAM_PROPAGATE_ERROR(prefetchAttributes(in, name));
            }
            if (typeId == Serialize::PrimitiveTypeIndex_v<std::string>) {
                if (in.readN(1) != "\"")
                    return STREAM_PARSE_ERROR(in, "Expected '\"'");
                in.setNextFormattedStringDelimiter('"');
            }
        } else {
            if (!mCurrentExtended) {
                std::string prefix = in.peekUntil(">");
                if (prefix.size() <= 1)
                    return STREAM_PARSE_ERROR(in, "Syntax error");
                if (name && prefix != "<" + std::string(name) + ">")
                    return STREAM_PARSE_ERROR(in, "Expected: '" << name << "'");
                in.readN(prefix.size() - 1);
            } else {
                mCurrentExtended = false;
            }
            std::string prefix = in.readN(1);
            if (prefix != ">")
                return STREAM_PARSE_ERROR(in, "Expected '>'");

            if (typeId == Serialize::PrimitiveTypeIndex_v<std::string>)
                in.setNextFormattedStringDelimiter('<');
        }
        return {};
    }

    void XMLFormatter::endPrimitive(SerializeOutStream &out, const char *name, uint8_t typeId)
    {
        if (!name)
            name = "Element";
        if (mCurrentExtended) {
            if (typeId == Serialize::PrimitiveTypeIndex_v<std::string>)
                out.writeUnformatted("\"");
        } else
            out.writeUnformatted("</" + std::string(name) + ">\n");
    }

    StreamResult XMLFormatter::endPrimitive(SerializeInStream &in, const char *name, uint8_t typeId)
    {
        if (mCurrentExtended) {
            if (mExtendedLookupPos != -1) {
                in.seek(mExtendedLookupPos);
                mExtendedLookupPos = -1;
            }
        } else {
            const char *cPrefix = ((typeId == Serialize::PrimitiveTypeIndex_v<std::string>) ? "/" : "</");
            std::string prefix = in.readUntil(">");
            if (prefix.size() <= 1)
                return STREAM_PARSE_ERROR(in, "Syntax error");
            if (name && prefix != cPrefix + std::string(name) + ">")
                return STREAM_PARSE_ERROR(in, "Expected: '" << name << "'");
        }
        return {};
    }

    std::string XMLFormatter::lookupFieldName(SerializeInStream &in)
    {
        std::string name = in.peekUntil("> ");
        if (!name.empty()) {
            if (name[0] != '<')
                throw 0;
            if (name[1] == '/')
                return {};
            return std::string { StringUtil::substr(name, 1, -1) };
        }
        return name;
    }

    void XMLFormatter::beginContainer(SerializeOutStream &out, const char *name, uint32_t size)
    {
        beginCompound(out, name);
    }

    void XMLFormatter::endContainer(SerializeOutStream &out, const char *name)
    {
        endCompound(out, name);
    }

    StreamResult XMLFormatter::beginContainer(SerializeInStream &in, const char *name, bool sized)
    {
        return beginCompound(in, name);
    }

    StreamResult XMLFormatter::endContainer(SerializeInStream &in, const char *name)
    {
        return endCompound(in, name);
    }

    bool XMLFormatter::hasContainerItem(SerializeInStream &in)
    {
        std::string prefix = in.peekN(2);
        if (prefix[0] != '<')
            throw 0;
        return prefix[1] != '/';
    }

    std::string XMLFormatter::indent()
    {
        return std::string(4 * mLevel, ' ');
    }

    StreamResult XMLFormatter::prefetchAttributes(SerializeInStream &in, const char *name)
    {
        std::string prefix = in.peekN(1);
        while (prefix != ">") {
            prefix = in.readUntil("=");
            if (name && prefix == name + "="s)
                return {};
            mPrefetchedAttributes.try_emplace(std::string { StringUtil::substr(prefix, 0, -1) }, in.tell());
            STREAM_PROPAGATE_ERROR(skipValue(in));
            prefix = in.peekN(1);
        }        
        if (name)
            return STREAM_PARSE_ERROR(in, "Missing attribute '" << name << "'");
        return {};
    }

    StreamResult XMLFormatter::skipValue(SerializeInStream &in)
    {
        std::string next = in.peekN(1);
        if (next == "\"") {
            in.readN(1);
            in.readUntil("\"");
        } else {
            float dummy; //TODO Is this catching all cases?
            STREAM_PROPAGATE_ERROR(in.readUnformatted(dummy));
        }
        return {};
    }

}
}
