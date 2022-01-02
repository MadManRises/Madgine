#pragma once

#include "streamstate.h"

namespace Engine {
namespace Serialize {

    struct META_EXPORT StreamError {
        StreamError(SerializeInStream &in, const std::string &msg, const char *file, size_t sourceLine);

        META_EXPORT friend std::ostream &operator<<(std::ostream &out, const StreamError &error);

        int mLineNumber = 0, mPosition = 0;
        std::string mMsg;
        std::string mNotes;
    };

    struct [[nodiscard]] StreamResult {
        StreamState mState = StreamState::OK;
        std::unique_ptr<StreamError> mError;

        META_EXPORT friend std::ostream &operator<<(std::ostream &out, const StreamResult &result);
    };

    struct META_EXPORT StreamResultBuilder {
        StreamState mType;
        SerializeInStream &mStream;
        const char *mFile;
        size_t mLine;
        std::stringstream mMsg;

        StreamResultBuilder(StreamState type, SerializeInStream &stream, const char *file, size_t line)
            : mType(type)
            , mStream(stream)
            , mFile(file)
            , mLine(line)
        {
        }

        operator StreamResult();

        template <typename T>
        StreamResultBuilder &&operator<<(T &&t) &&
        {
            mMsg << std::forward<T>(t);
            return std::move(*this);
        }
    };

#define STREAM_ERROR(Stream, Type, ...) ::Engine::Serialize::StreamResultBuilder { Type, Stream, __FILE__, __LINE__ } << __VA_ARGS__;
#define STREAM_PARSE_ERROR(Stream, ...) STREAM_ERROR(Stream, ::Engine::Serialize::StreamState::PARSE_ERROR, __VA_ARGS__);
#define STREAM_PERMISSION_ERROR(Stream, ...) STREAM_ERROR(Stream, ::Engine::Serialize::StreamState::PERMISSION_ERROR, __VA_ARGS__);
#define STREAM_INTEGRITY_ERROR(Stream, ...) STREAM_ERROR(Stream, ::Engine::Serialize::StreamState::INTEGRITY_ERROR, __VA_ARGS__);

#define STREAM_PROPAGATE_ERROR(expr)                                                                              \
    if (::Engine::Serialize::StreamResult _result = (expr); _result.mState != ::Engine::Serialize::StreamState::OK) \
    return _result

}
}