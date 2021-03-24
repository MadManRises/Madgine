#include "Meta/metalib.h"

#include "jsonformatter.h"

#include "../streams/serializestream.h"

#include "Generic/stringutil.h"

#include "../primitivetypes.h"

namespace Engine {
namespace Serialize {

    JSONFormatter::JSONFormatter()
        : Formatter(false, true)
    {
        mParseLevel.emplace(false);
    }

    void JSONFormatter::setupStream(std::istream &in)
    {
        in >> std::boolalpha;
    }

    void JSONFormatter::setupStream(std::ostream &out)
    {
        out << std::boolalpha;
    }

    void JSONFormatter::beginExtended(SerializeOutStream &out, const char *name, size_t count)
    {
        if (!mCurrentExtended) {
            if (mAfterItem) {
                out.writeUnformatted(",");
                mAfterItem = false;
            }
            out.writeUnformatted("\n" + indent());
            if (!mParseLevel.top().mIsContainer) {
                if (!name)
                    name = "Item";
                out.writeUnformatted("\"" + std::string(name) + "\" : ");
            }
            mParseLevel.emplace(false);
            out.writeUnformatted("{\n");
            mCurrentExtended = true;
            ++mLevel;
            out.writeUnformatted(indent() + "\"__extended\" : {");
            ++mLevel;
        }
        assert(mCurrentExtendedCount == 0);
        assert(count > 0);
        mCurrentExtendedCount = count;
    }

    StreamResult JSONFormatter::beginExtended(SerializeInStream &in, const char *name, size_t count)
    {
        if (!mCurrentExtended) {
            if (!mParseLevel.top().mIsContainer) {
                auto it = mParseLevel.top().mPrefetchedFields.find(name);
                if (it != mParseLevel.top().mPrefetchedFields.end()) {
                    mParseLevel.top().mLookupPos = in.tell();
                    in.seek(it->second);
                    mParseLevel.top().mPrefetchedFields.erase(it);
                } else {
                    if (mAfterItem) {
                        if (in.readN(1) != ",")
                            return STREAM_PARSE_ERROR(in, "Expected ','");
                        mAfterItem = false;
                    }
                    STREAM_PROPAGATE_ERROR(readFieldName(in, name));
                }
            } else {
                if (mAfterItem) {
                    if (in.readN(1) != ",")
                        return STREAM_PARSE_ERROR(in, "Expected ','");
                    mAfterItem = false;
                }
            }
            mParseLevel.emplace(false);
            if (in.readN(1) != "{")
                return STREAM_PARSE_ERROR(in, "Expected '{'");
            mCurrentExtended = true;
            STREAM_PROPAGATE_ERROR(readFieldName(in, "__extended"));
            if (in.readN(1) != "{")
                return STREAM_PARSE_ERROR(in, "Expected '{'");
        }
        assert(mCurrentExtendedCount == 0);
        assert(count > 0);
        mCurrentExtendedCount = count;
        return {};
    }

    void JSONFormatter::beginCompound(SerializeOutStream &out, const char *name)
    {
        if (!mCurrentExtended) {
            if (mAfterItem) {
                out.writeUnformatted(",");
                mAfterItem = false;
            }
            out.writeUnformatted("\n" + indent());
            if (!mParseLevel.top().mIsContainer) {
                if (!name)
                    name = "Item";
                out.writeUnformatted("\"" + std::string(name) + "\" : ");
            }
            mParseLevel.emplace(false);
            out.writeUnformatted("{");
            ++mLevel;
        } else {
            assert(mCurrentExtendedCount == 0);
            --mLevel;
            out.writeUnformatted("\n" + indent() + "}");
            mAfterItem = true;
        }
        mCurrentExtended = false;
    }

    StreamResult JSONFormatter::beginCompound(SerializeInStream &in, const char *name)
    {
        if (!mCurrentExtended) {
            if (!mParseLevel.top().mIsContainer) {
                auto it = mParseLevel.top().mPrefetchedFields.find(name);
                if (it != mParseLevel.top().mPrefetchedFields.end()) {
                    mParseLevel.top().mLookupPos = in.tell();
                    in.seek(it->second);
                    mParseLevel.top().mPrefetchedFields.erase(it);
                } else {
                    if (mAfterItem) {
                        if (in.readN(1) != ",")
                            return STREAM_PARSE_ERROR(in, "Expected ','");
                        mAfterItem = false;
                    }
                    STREAM_PROPAGATE_ERROR(readFieldName(in, name));
                }
            } else {
                if (mAfterItem) {
                    if (in.readN(1) != ",")
                        return STREAM_PARSE_ERROR(in, "Expected ','");
                    mAfterItem = false;
                }
            }

            mParseLevel.emplace(false);
            if (in.readN(1) != "{")
                return STREAM_PARSE_ERROR(in, "Expected '{'");
        } else {
            assert(mCurrentExtendedCount == 0);
            if (in.readN(1) != "}")
                return STREAM_PARSE_ERROR(in, "Expected '}'");
            mAfterItem = true;
        }
        mCurrentExtended = false;
        return {};
    }

    void JSONFormatter::endCompound(SerializeOutStream &out, const char *name)
    {
        if (mAfterItem) {
            mAfterItem = false;
        }
        --mLevel;
        out.writeUnformatted("\n" + indent() + "}");
        assert(!mParseLevel.top().mIsContainer);
        mParseLevel.pop();
        mAfterItem = true;
        mLastPrimitive = false;
    }

    StreamResult JSONFormatter::endCompound(SerializeInStream &in, const char *name)
    {
        if (mAfterItem) {
            mAfterItem = false;
        }
        if (in.readN(1) != "}")
            return STREAM_PARSE_ERROR(in, "Expected '}'");
        assert(!mParseLevel.top().mIsContainer);
        mParseLevel.pop();
        if (mParseLevel.top().mLookupPos != -1) {
            in.seek(mParseLevel.top().mLookupPos);
            mParseLevel.top().mLookupPos = -1;
        }
        mAfterItem = true;

        return {};
    }

    void JSONFormatter::beginPrimitive(SerializeOutStream &out, const char *name, uint8_t typeId)
    {
        if (mAfterItem) {
            out.writeUnformatted(",");
            mAfterItem = false;
        }
        if (mCurrentExtendedCount > 0) {
            assert(mCurrentExtended);
            --mCurrentExtendedCount;
        }
        if (!mParseLevel.top().mIsContainer) {
            if (!name)
                name = "Element";
            out.writeUnformatted("\n" + indent() + "\"" + name + "\" : ");
        }
        if (typeId == Serialize::PrimitiveTypeIndex_v<std::string>)
            out.writeUnformatted("\"");
    }

    StreamResult JSONFormatter::beginPrimitive(SerializeInStream &in, const char *name, uint8_t typeId)
    {
        if (mCurrentExtendedCount > 0) {
            assert(mCurrentExtended);
            --mCurrentExtendedCount;
        }
        if (!mParseLevel.top().mIsContainer) {
            auto it = mParseLevel.top().mPrefetchedFields.find(name);
            if (it != mParseLevel.top().mPrefetchedFields.end()) {
                mParseLevel.top().mLookupPos = in.tell();
                in.seek(it->second);
                mParseLevel.top().mPrefetchedFields.erase(it);
            } else {
                if (mAfterItem) {
                    if (in.readN(1) != ",")
                        throw 0;
                    mAfterItem = false;
                }
                STREAM_PROPAGATE_ERROR(readFieldName(in, name));
            }
        } else {
            if (mAfterItem) {
                if (in.readN(1) != ",")
                    throw 0;
                mAfterItem = false;
            }
        }
        if (typeId == Serialize::PrimitiveTypeIndex_v<std::string>) {
            if (in.readN(1) != "\"")
                return STREAM_PARSE_ERROR(in, "Expected '\"'");
            in.setNextFormattedStringDelimiter('"');
        }
        return {};
    }

    void JSONFormatter::endPrimitive(SerializeOutStream &out, const char *name, uint8_t typeId)
    {
        if (typeId == Serialize::PrimitiveTypeIndex_v<std::string>)
            out.writeUnformatted("\"");
        mAfterItem = true;
        mLastPrimitive = true;
    }

    StreamResult JSONFormatter::endPrimitive(SerializeInStream &in, const char *name, uint8_t typeId)
    {
        if (mParseLevel.top().mLookupPos != -1) {
            in.seek(mParseLevel.top().mLookupPos);
            mParseLevel.top().mLookupPos = -1;
        }
        mAfterItem = true;
        return {};
    }

    StreamResult JSONFormatter::readFieldName(SerializeInStream &in, const char *name)
    {
        std::string prefix = in.readUntil(":");
        std::string_view trimmed { prefix.c_str(), prefix.size() - 1 };
        trimmed = StringUtil::trim(trimmed);
        while (name && StringUtil::substr(trimmed, 1, -1) != name) {
            mParseLevel.top().mPrefetchedFields.try_emplace(std::string { StringUtil::substr(trimmed, 1, -1) }, in.tell());
            STREAM_PROPAGATE_ERROR(skipObject(in));
            std::string next = in.readN(1);
            if (next == "}")
                return STREAM_PARSE_ERROR(in, "Expected key \"" << name << "\" not found");
            if (next != ",")
                return STREAM_PARSE_ERROR(in, "Expected ',', Found '" << next << "'.");

            prefix = in.readUntil(":");
            trimmed = { prefix.c_str(), prefix.size() - 1 };
            trimmed = StringUtil::trim(trimmed);
        }
        if (!StringUtil::startsWith(trimmed, "\"") || !StringUtil::endsWith(trimmed, "\""))
            return STREAM_PARSE_ERROR(in, "Expected key in '\"\"'");
        if (trimmed.size() <= 1)
            return STREAM_PARSE_ERROR(in, "Expected key");
        return {};
    }

    std::string JSONFormatter::lookupFieldName(SerializeInStream &in)
    {
        pos_type pos = in.tell();
        std::string controlChar = in.readN(1);
        if (controlChar == "}") {
            in.seek(pos);
            return {};
        }
        if (mAfterItem) {
            if (controlChar != ",")
                throw 0;
        } else {
            in.seek(pos);
        }
        std::string prefix = in.readUntil(":");
        std::string_view trimmed { prefix.c_str(), prefix.size() - 1 };
        trimmed = StringUtil::trim(trimmed);
        if (!StringUtil::startsWith(trimmed, "\"") || !StringUtil::endsWith(trimmed, "\""))
            throw 0;
        if (trimmed.size() <= 1)
            throw 0;
        in.seek(pos);
        return std::string { StringUtil::substr(trimmed, 1, -1) };
    }

    void JSONFormatter::beginContainer(SerializeOutStream &out, const char *name, uint32_t size)
    {
        if (mAfterItem) {
            out.writeUnformatted(",\n");
            mAfterItem = false;
        }
        if (!name)
            name = "Item";
        out.writeUnformatted(indent() + "\"" + std::string(name) + "\" : [");
        ++mLevel;
        mParseLevel.emplace(true);
    }

    StreamResult JSONFormatter::beginContainer(SerializeInStream &in, const char *name, bool sized)
    {
        if (mAfterItem) {
            if (in.readN(1) != ",")
                return STREAM_PARSE_ERROR(in, "Expected ','");
            mAfterItem = false;
        }
        STREAM_PROPAGATE_ERROR(readFieldName(in, name));
        std::string bracket = sized ? in.peekN(1) : in.readN(1);
        if (bracket != "[")
            return STREAM_PARSE_ERROR(in, "Expected '['");
        mParseLevel.emplace(true);
        return {};
    }

    void JSONFormatter::endContainer(SerializeOutStream &out, const char *name)
    {
        assert(mParseLevel.top().mIsContainer);
        mParseLevel.pop();
        if (mAfterItem) {
            mAfterItem = false;
        }
        --mLevel;
        if (!mLastPrimitive)
            out.writeUnformatted("\n" + indent());
        out.writeUnformatted("]");
        mAfterItem = true;
    }

    StreamResult JSONFormatter::endContainer(SerializeInStream &in, const char *name)
    {
        assert(mParseLevel.top().mIsContainer);
        mParseLevel.pop();
        if (mAfterItem) {
            mAfterItem = false;
        }
        if (in.readN(1) != "]")
            return STREAM_PARSE_ERROR(in, "Expected ']'");
        mAfterItem = true;
        return {};
    }

    bool JSONFormatter::hasContainerItem(SerializeInStream &in)
    {
        std::string prefix = in.peekN(1);
        if (prefix[0] == '[') {
            in.readN(1);
            return in.peekN(1) != "]";
        }
        if (prefix[0] == ',')
            return true;
        if (prefix[0] != ']')
            throw 0;
        return false;
    }

    std::string JSONFormatter::indent()
    {
        return std::string(4 * mLevel, ' ');
    }

    StreamResult JSONFormatter::skipObject(SerializeInStream &in)
    {
        switch (in.readN(1)[0]) {
        case '[': {
            std::string next;
            do {
                STREAM_PROPAGATE_ERROR(skipObject(in));
                next = in.readN(1);
            } while (next == ",");
            if (next != "]")
                return STREAM_PARSE_ERROR(in, "Expected ']', Found '" << next << "'.");
        } break;
        case '{': {
            std::string next;
            do {
                STREAM_PROPAGATE_ERROR(readFieldName(in, nullptr));
                STREAM_PROPAGATE_ERROR(skipObject(in));
                next = in.readN(1);
            } while (next == ",");
            if (next != "}")
                return STREAM_PARSE_ERROR(in, "Expected '}', Found '" << next << "'.");
        } break;
        default: {
            std::string value = in.readUntil("}],");
            in.seek(in.tell() - off_type { 1 });
        }
        }
        return {};
    }

}
}
