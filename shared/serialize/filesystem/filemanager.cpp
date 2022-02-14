#include "filesystemlib.h"

#include "filemanager.h"

#include "Interfaces/filesystem/path.h"

#include "Interfaces/filesystem/api.h"

#include "Meta/serialize/streams/serializestream.h"

#include "Meta/serialize/formatter.h"

#include "Meta/serialize/streams/serializestreamdata.h"

#include "Meta/serialize/streams/formattedserializestream.h"

namespace Engine {
namespace Filesystem {

    FileManager::FileManager(const std::string &name)
        : SerializeManager(name)
    {
    }

    Serialize::FormattedSerializeStream FileManager::openRead(const Path &path, std::unique_ptr<Serialize::Formatter> format)
    {
        assert(!getSlaveStreamData());

        InStream stream = openFileRead(path, format->mBinary);
        if (stream) {
            Serialize::FormattedSerializeStream in { stream.release(), std::make_unique<Serialize::SerializeStreamData>(std::move(format), *this, createStreamId()) };
            setSlaveStreamData(&in.data());

            return in;
        } else {
            return {};
        }
    }

    Serialize::FormattedSerializeStream FileManager::openWrite(const Path &path, std::unique_ptr<Serialize::Formatter> format)
    {
        OutStream stream = openFileWrite(path, format->mBinary);
        if (stream) {
            return Serialize::FormattedSerializeStream { stream.release(), std::make_unique<Serialize::SerializeStreamData>(std::move(format), *this, createStreamId()) };           
        } else {
            return {};
        }        
    }

}
}
