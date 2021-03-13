#include "../../metalib.h"

#include "streamerror.h"

#include "serializestream.h"

#include <iostream>

namespace Engine {
namespace Serialize {

    StreamError::StreamError(SerializeInStream &in, const std::string &msg, const char *file, size_t sourceLine)
    {
        std::stringstream ss;
        std::stringstream notes;

        std::ios_base::iostate state = in.state();
        in.clear();

        mPosition = in.tell();

        ss << "ERROR: (";

        if (!in.isBinary()) {
            in.seek(0);
            pos_type lastNewLine = 0;
            mLineNumber = 1;

            std::istreambuf_iterator it = in.iterator();
            for (int i = 0; i < mPosition; ++i) {
                char c = *it++;
                if (c == '\n') {
                    ++mLineNumber;
                    lastNewLine = i;
                }
            }
            mPosition -= lastNewLine;

            ss << mLineNumber << ", ";

            if (in) {
                std::string area;
                in.InStream::operator>>(area);
                notes << "Note: next to '" << area << "'\n";
            }
        }

        notes << "Note: in file '" << file << "':" << sourceLine << "\n";

        ss << mPosition << "): " << msg;

        mMsg = ss.str();
        mNotes = notes.str();

        in.setState(state);        
    }

    std::ostream &operator<<(std::ostream &out, const StreamError &error)
    {
        return out << error.mMsg << "\n"
                   << error.mNotes;
    }

    StreamResultBuilder::operator StreamResult()
    {
        return {
            mType,
            mType == StreamState::OK ? std::unique_ptr<StreamError> {} : std::make_unique<StreamError>(mStream, mMsg.str(), mFile, mLine)
        };
    }

}
}