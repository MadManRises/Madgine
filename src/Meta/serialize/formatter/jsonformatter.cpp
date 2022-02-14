#include "Meta/metalib.h"

#include "jsonformatter.h"

#include "../streams/serializestream.h"

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

    void JSONFormatter::beginExtendedWrite(const char *name, size_t count)
    {
        if (!mCurrentExtended) {
            if (mAfterItem) {
                mOut << ",";
                mAfterItem = false;
            }
            mOut << "\n" << indent();
            if (!mParseLevel.top().mIsContainer) {
                if (!name)
                    name = "Item";
                mOut << "\"" << name << "\" : ";
            }
            mParseLevel.emplace(false);
            mOut << "{\n";
            mCurrentExtended = true;
            ++mLevel;
            mOut << indent() << "\"__extended\" : {";
            ++mLevel;
        }
        assert(mCurrentExtendedCount == 0);
        assert(count > 0);
        mCurrentExtendedCount = count;
    }

    StreamResult JSONFormatter::beginExtendedRead(const char *name, size_t count)
    {
        if (!mCurrentExtended) {
            if (!mParseLevel.top().mIsContainer) {
                auto it = mParseLevel.top().mPrefetchedFields.find(name);
                if (it != mParseLevel.top().mPrefetchedFields.end()) {
                    mParseLevel.top().mLookupPos = mIn.tell();
                    mIn.seek(it->second);
                    mParseLevel.top().mPrefetchedFields.erase(it);
                } else {
                    if (mAfterItem) {
                        FORMATTER_EXPECT(",");
                        mAfterItem = false;
                    }
                    STREAM_PROPAGATE_ERROR(readFieldName(name));
                }
            } else {
                if (mAfterItem) {
                    FORMATTER_EXPECT(",");
                    mAfterItem = false;
                }
            }
            mParseLevel.emplace(false);
            FORMATTER_EXPECT("{");
            mCurrentExtended = true;
            STREAM_PROPAGATE_ERROR(readFieldName("__extended"));
            FORMATTER_EXPECT("{");
        }
        assert(mCurrentExtendedCount == 0);
        assert(count > 0);
        mCurrentExtendedCount = count;
        return {};
    }

    void JSONFormatter::beginCompoundWrite(const char *name)
    {
        if (!mCurrentExtended) {
            if (mAfterItem) {
                mOut << ",";
                mAfterItem = false;
            }
            mOut << "\n" << indent();
            if (!mParseLevel.top().mIsContainer) {
                if (!name)
                    name = "Item";
                mOut << "\"" << name << "\" : ";
            }
            mParseLevel.emplace(false);
            mOut << "{";
            ++mLevel;
        } else {
            assert(mCurrentExtendedCount == 0);
            --mLevel;
            mOut << "\n" << indent() << "}";
            mAfterItem = true;
        }
        mCurrentExtended = false;
    }

    StreamResult JSONFormatter::beginCompoundRead(const char *name)
    {
        if (!mCurrentExtended) {
            if (!mParseLevel.top().mIsContainer) {
                auto it = mParseLevel.top().mPrefetchedFields.find(name);
                if (it != mParseLevel.top().mPrefetchedFields.end()) {
                    mParseLevel.top().mLookupPos = mIn.tell();
                    mIn.seek(it->second);
                    mParseLevel.top().mPrefetchedFields.erase(it);
                } else {
                    if (mAfterItem) {
                        FORMATTER_EXPECT(",");
                        mAfterItem = false;
                    }
                    STREAM_PROPAGATE_ERROR(readFieldName(name));
                }
            } else {
                if (mAfterItem) {
                    FORMATTER_EXPECT(",");
                    mAfterItem = false;
                }
            }

            mParseLevel.emplace(false);
            FORMATTER_EXPECT("{");
        } else {
            assert(mCurrentExtendedCount == 0);
            FORMATTER_EXPECT("}");
            mAfterItem = true;
        }
        mCurrentExtended = false;
        return {};
    }

    void JSONFormatter::endCompoundWrite(const char *name)
    {
        if (mAfterItem) {
            mAfterItem = false;
        }
        --mLevel;
        mOut << "\n" << indent() << "}";
        assert(!mParseLevel.top().mIsContainer);
        mParseLevel.pop();
        mAfterItem = true;
        mLastPrimitive = false;
    }

    StreamResult JSONFormatter::endCompoundRead(const char *name)
    {
        if (mAfterItem) {
            mAfterItem = false;
        }
        FORMATTER_EXPECT("}");
        assert(!mParseLevel.top().mIsContainer);
        mParseLevel.pop();
        if (mParseLevel.top().mLookupPos != -1) {
            mIn.seek(mParseLevel.top().mLookupPos);
            mParseLevel.top().mLookupPos = -1;
        }
        mAfterItem = true;

        return {};
    }

    void JSONFormatter::beginPrimitiveWrite(const char *name, uint8_t typeId)
    {
        if (mAfterItem) {
            mOut << ",";
            mAfterItem = false;
        }
        if (mCurrentExtendedCount > 0) {
            assert(mCurrentExtended);
            --mCurrentExtendedCount;
        }
        if (!mParseLevel.top().mIsContainer) {
            if (!name)
                name = "Element";
            mOut << "\n" << indent() << "\"" << name << "\" : ";
        }
        if (typeId == Serialize::PrimitiveTypeIndex_v<std::string> || typeId == Serialize::PrimitiveTypeIndex_v<ByteBuffer>)
            mOut << "\"";
    }

    StreamResult JSONFormatter::beginPrimitiveRead(const char *name, uint8_t typeId)
    {
        if (mCurrentExtendedCount > 0) {
            assert(mCurrentExtended);
            --mCurrentExtendedCount;
        }
        if (!mParseLevel.top().mIsContainer) {
            auto it = mParseLevel.top().mPrefetchedFields.find(name);
            if (it != mParseLevel.top().mPrefetchedFields.end()) {
                mParseLevel.top().mLookupPos = mIn.tell();
                mIn.seek(it->second);
                mParseLevel.top().mPrefetchedFields.erase(it);
            } else {
                if (mAfterItem) {
                    FORMATTER_EXPECT(",");
                    mAfterItem = false;
                }
                STREAM_PROPAGATE_ERROR(readFieldName(name));
            }
        } else {
            if (mAfterItem) {
                FORMATTER_EXPECT(",");
                mAfterItem = false;
            }
        }
        if (typeId == Serialize::PrimitiveTypeIndex_v<std::string> || typeId == Serialize::PrimitiveTypeIndex_v<ByteBuffer>) {
            FORMATTER_EXPECT("\"");
            mNextStringDelimiter = "\"";
        }
        return {};
    }

    void JSONFormatter::endPrimitiveWrite(const char *name, uint8_t typeId)
    {
        if (typeId == Serialize::PrimitiveTypeIndex_v<std::string> || typeId == Serialize::PrimitiveTypeIndex_v<ByteBuffer>)
            mOut << "\"";
        mAfterItem = true;
        mLastPrimitive = true;
    }

    StreamResult JSONFormatter::endPrimitiveRead(const char *name, uint8_t typeId)
    {
        if (mParseLevel.top().mLookupPos != -1) {
            mIn.seek(mParseLevel.top().mLookupPos);
            mParseLevel.top().mLookupPos = -1;
        }
        mAfterItem = true;
        return {};
    }

    StreamResult JSONFormatter::readFieldName(const char *name)
    {
        std::string prefix;
        STREAM_PROPAGATE_ERROR(mIn.readUntil(prefix, ":"));
        std::string_view trimmed = StringUtil::trim(StringUtil::substr(prefix, 0, -1));
        while (name && StringUtil::substr(trimmed, 1, -1) != name) {
            mParseLevel.top().mPrefetchedFields.try_emplace(std::string { StringUtil::substr(trimmed, 1, -1) }, mIn.tell());
            STREAM_PROPAGATE_ERROR(skipObject());
            FORMATTER_EXPECT(",");
            STREAM_PROPAGATE_ERROR(mIn.readUntil(prefix, ":"));
            trimmed = { prefix.c_str(), prefix.size() - 1 };
            trimmed = StringUtil::trim(trimmed);
        }
        if (!StringUtil::startsWith(trimmed, "\"") || !StringUtil::endsWith(trimmed, "\""))
            return STREAM_PARSE_ERROR(mIn, mBinary, "Expected key in '\"\"'");
        if (trimmed.size() <= 1)
            return STREAM_PARSE_ERROR(mIn, mBinary, "Expected key");
        return {};
    }

    StreamResult JSONFormatter::lookupFieldName(std::string &name)
    {
        name.clear();
        pos_type pos = mIn.tell();
        std::string controlChar;
        STREAM_PROPAGATE_ERROR(mIn.readN(controlChar, 1));
        if (controlChar == "}") {
            mIn.seek(pos);
            return {};
        }
        if (mAfterItem) {
            if (controlChar != ",")
                throw 0;
        } else {
            mIn.seek(pos);
        }
        std::string prefix;
        STREAM_PROPAGATE_ERROR(mIn.readUntil(prefix, ":"));
        std::string_view trimmed { prefix.c_str(), prefix.size() - 1 };
        trimmed = StringUtil::trim(trimmed);
        if (!StringUtil::startsWith(trimmed, "\"") || !StringUtil::endsWith(trimmed, "\""))
            throw 0;
        if (trimmed.size() <= 1)
            throw 0;
        mIn.seek(pos);
        name = StringUtil::substr(trimmed, 1, -1);
        return {};
    }

    void JSONFormatter::beginContainerWrite(const char *name, uint32_t size)
    {
        if (mAfterItem) {
            mOut << ",\n";
            mAfterItem = false;
        }
        if (!name)
            name = "Item";
        mOut << indent() << "\"" << name << "\" : [";
        ++mLevel;
        mParseLevel.emplace(true);
    }

    StreamResult JSONFormatter::beginContainerRead(const char *name, bool sized)
    {
        if (mAfterItem) {
            FORMATTER_EXPECT(",");
            mAfterItem = false;
        }
        STREAM_PROPAGATE_ERROR(readFieldName(name));
        std::string bracket;
        STREAM_PROPAGATE_ERROR(sized ? mIn.peekN(bracket, 1) : mIn.readN(bracket, 1));
        if (bracket != "[")
            return STREAM_PARSE_ERROR(mIn, mBinary, "Expected '['");
        mParseLevel.emplace(true);
        return {};
    }

    void JSONFormatter::endContainerWrite(const char *name)
    {
        assert(mParseLevel.top().mIsContainer);
        mParseLevel.pop();
        if (mAfterItem) {
            mAfterItem = false;
        }
        --mLevel;
        if (!mLastPrimitive)
            mOut << "\n" << indent();
        mOut << "]";
        mAfterItem = true;
    }

    StreamResult JSONFormatter::endContainerRead(const char *name)
    {
        assert(mParseLevel.top().mIsContainer);
        mParseLevel.pop();
        if (mAfterItem) {
            mAfterItem = false;
        }
        FORMATTER_EXPECT("]");
        mAfterItem = true;
        return {};
    }

    bool JSONFormatter::hasContainerItem()
    {
        std::string prefix;
        if (mIn.peekN(prefix, 1).mState != StreamState::OK)
            return false;
        if (prefix[0] == '[') {
            if (mIn.readN(prefix, 1).mState != StreamState::OK)
                return false;
            if (mIn.peekN(prefix, 1).mState != StreamState::OK)
                return false;
            return prefix != "]";
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

    StreamResult JSONFormatter::skipObject()
    {
        std::string next;
        STREAM_PROPAGATE_ERROR(mIn.readN(next, 1));
        switch (next[0]) {
        case '[': {
            do {
                STREAM_PROPAGATE_ERROR(skipObject());
                STREAM_PROPAGATE_ERROR(mIn.readN(next, 1));
            } while (next == ",");
            if (next != "]")
                return STREAM_PARSE_ERROR(mIn, mBinary, "Expected ']', Found '" << next << "'.");
        } break;
        case '{': {
            do {
                STREAM_PROPAGATE_ERROR(readFieldName(nullptr));
                STREAM_PROPAGATE_ERROR(skipObject());
                STREAM_PROPAGATE_ERROR(mIn.readN(next, 1));
            } while (next == ",");
            if (next != "}")
                return STREAM_PARSE_ERROR(mIn, mBinary, "Expected '}', Found '" << next << "'.");
        } break;
        default: {
            std::string value;
            STREAM_PROPAGATE_ERROR(mIn.readUntil(value, "}],"));
            mIn.seek(mIn.tell() - off_type { 1 });
        }
        }
        return {};
    }

}
}
