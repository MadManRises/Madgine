#include "../metalib.h"

#include "formatter.h"

#include "streams/serializestream.h"
#include "streams/streamresult.h"

#include "Generic/bytebuffer.h"

#include "base64/base64.h"

#include "serializemanager.h"

namespace Engine {
namespace Serialize {

    SerializeStream &Formatter::stream()
    {
        return mStream;
    }

    void Formatter::setupStream(SerializeStream stream) {
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

    StreamResult Formatter::read(std::string &s)
    {
        if (mBinary) {
            mStream.read(s);
        } else if (mNextStringDelimiter) {
            STREAM_PROPAGATE_ERROR(mStream.readUntil(s, mNextStringDelimiter));
            s.resize(s.size() - 1);
            mNextStringDelimiter = nullptr;
        } else {
            mStream >> s;
        }
        return {};
    }

    StreamResult Formatter::expect(std::string_view expected)
    {
        std::string actual;
        STREAM_PROPAGATE_ERROR(mStream.readN(actual, expected.size()));
        if (actual != expected)
            return STREAM_PARSE_ERROR(mStream, mBinary, "Expected '" << expected << "', Got '" << actual << "'");
        return {};
    }

}
}