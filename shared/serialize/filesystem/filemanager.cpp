#include "filesystemlib.h"

#include "filemanager.h"

#include "Interfaces/filesystem/path.h"

#include "Interfaces/filesystem/api.h"

#include "Meta/serialize/streams/serializestream.h"

#include "Meta/serialize/formatter.h"

#include "Meta/serialize/streams/serializestreambuf.h"

namespace Engine {
namespace Filesystem {

    FileManager::FileManager(const std::string &name)
        : SerializeManager(name)
    {
    }

    Serialize::SerializeInStream FileManager::openRead(const Path &path, std::unique_ptr<Serialize::Formatter> format)
    {
        assert(!getSlaveStreamData());

        InStream stream = openFileRead(path, format->mBinary);
        if (stream) {
            Serialize::SerializeInStream in { stream.release(), std::make_unique<Serialize::SerializeStreamData>(std::move(format), *this, createStreamId()) };
            setSlaveStreamData(&in.data());

            return in;
        } else {
            return {};
        }
    }

    Serialize::SerializeOutStream FileManager::openWrite(const Path &path, std::unique_ptr<Serialize::Formatter> format)
    {
        OutStream stream = openFileWrite(path, format->mBinary);
        if (stream) {
            return Serialize::SerializeOutStream { stream.release(), std::make_unique<Serialize::SerializeStreamData>(std::move(format), *this, createStreamId()) };           
        } else {
            return {};
        }        
    }

}
}
