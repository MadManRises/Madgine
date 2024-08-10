#include "behaviorlib.h"

#include "behaviorerror.h"

namespace Engine {

BehaviorError::BehaviorError(GenericResult result)
    : mResult(result)
{
}

BehaviorError::BehaviorError(BehaviorResult result, const std::string &msg, const char *file, size_t sourceLine)
    : mResult(result)
    , mMsg(msg)
    , mFile(file)
    , mLineNumber(sourceLine)
{
}

std::ostream &operator<<(std::ostream &out, const BehaviorError &error)
{
    return out << error.mResult << "\n"
               << error.mMsg << "\n"
               << error.mNotes;
}

Log::LogDummy tag_invoke(Log::log_for_t, Log::MessageType lvl, BehaviorError &error, Log::Log *log)
{
    return Log::LogDummy { lvl, error.mFile, static_cast<size_t>(error.mLineNumber), log };
}

BehaviorErrorBuilder::BehaviorErrorBuilder(BehaviorResult type, const char *file, size_t line)
    : mType(type)
    , mFile(file)
    , mLine(line)
{
}

BehaviorErrorBuilder::operator BehaviorError()
{
    return {
        mType, mMsg.str(), mFile, mLine
    };
}

}