#include "../../metalib.h"

#include "formatter.h"

#include "base64/base64.h"

namespace Engine {
namespace Serialize {

    std::array<ctype::mask, 256> generateMask(ctype::mask mask, std::initializer_list<std::tuple<char, ctype::mask, ctype::mask>> specialCharacters)
    {
        std::array<ctype::mask, 256> result;

        std::transform(ctype::classic_table(), ctype::classic_table() + 256, result.data(), [=](ctype::mask value) { return value & mask; });

        for (auto [c, orMask, andMask] : specialCharacters) {
            result[c] &= andMask;
            result[c] |= orMask;
        }

        return result;
    }

    SerializeStream &Formatter::stream()
    {
        return mStream;
    }

    void Formatter::setupStream(SerializeStream stream)
    {
        assert(!mStream);
        mStream = std::move(stream);
        setupStream(mStream.stream());
    }

    StreamResult Formatter::beginExtendedRead(const char *name, size_t count)
    {
        return {};
    }

    StreamResult Formatter::beginCompoundRead(const char *name)
    {
        return {};
    }

    StreamResult Formatter::endCompoundRead(const char *name)
    {
        return {};
    }

    StreamResult Formatter::beginPrimitiveRead(const char *name, uint8_t typeId)
    {
        return {};
    }

    StreamResult Formatter::endPrimitiveRead(const char *name, uint8_t typeId)
    {
        return {};
    }

    StreamResult Formatter::beginContainerRead(const char *name, bool sized)
    {
        return {};
    }

    StreamResult Formatter::endContainerRead(const char *name)
    {
        return {};
    }

    StreamResult Formatter::beginHeaderRead()
    {
        return {};
    }

    StreamResult Formatter::endHeaderRead()
    {
        return {};
    }

    StreamResult Formatter::beginMessageRead()
    {
        return {};
    }

    StreamResult Formatter::endMessageRead()
    {
        return {};
    }

    StreamResult Formatter::read(ByteBuffer &b)
    {
        if (mBinary) {
            return mStream.read(b);
        } else {
            std::string base64Encoded;
            STREAM_PROPAGATE_ERROR(read(base64Encoded));
            if (!Base64::decode(b, base64Encoded))
                return STREAM_PARSE_ERROR(mStream, mBinary) << "Invalid Base64 String '" << base64Encoded << "'";
            return {};
        }
    }

    void Formatter::write(const ByteBuffer &b)
    {
        if (mBinary) {
            mStream.write(b);
        } else {
            mStream << Base64::encode(b);
        }
    }

    StreamResult Formatter::expect(std::string_view expected)
    {
        std::string actual;
        STREAM_PROPAGATE_ERROR(mStream.readN(actual, expected.size()));
        if (actual != expected)
            return STREAM_PARSE_ERROR(mStream, mBinary) << "Expected '" << expected << "', Got '" << actual << "'";
        return {};
    }

    void Formatter::pushLocale(const std::locale &locale, bool skipWs)
    {
        mLocaleStack.push({ mStream.imbue(locale), mStream.isSkipWs() });
        if (skipWs)
            mStream << std::skipws;
        else
            mStream << std::noskipws;
    }

    void Formatter::popLocale()
    {
        mStream.imbue(mLocaleStack.top().first);
        if (mLocaleStack.top().second)
            mStream << std::skipws;
        else
            mStream << std::noskipws;
        mLocaleStack.pop();
    }

}
}