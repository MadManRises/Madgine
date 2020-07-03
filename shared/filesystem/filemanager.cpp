#include "filesystemlib.h"

#include "filemanager.h"

#include "Interfaces/filesystem/path.h"

#include "Interfaces/filesystem/api.h"

#include "Modules/serialize/streams/wrappingserializestreambuf.h"

#include "Modules/serialize/streams/serializestream.h"

#include "Modules/serialize/formatter.h"

namespace Engine {
namespace Filesystem {

    FileManager::FileManager(const std::string &name)
        : SerializeManager(name)
    {
    }

    Serialize::SerializeInStream FileManager::openRead(const Path &path, std::unique_ptr<Serialize::Formatter> format)
    {
        assert(!getSlaveStreambuf());

        InStream stream = openFileRead(path, format->mBinary);
        if (stream) {
            Serialize::SerializeInStream in { std::make_unique<Serialize::WrappingSerializeStreambuf>(stream.release(), std::move(format), *this, createStreamId()) };
            setSlaveStreambuf(&in.buffer());

            return in;
        } else {
            return {};
        }
    }

    Serialize::SerializeOutStream FileManager::openWrite(const Path &path, std::unique_ptr<Serialize::Formatter> format)
    {
        OutStream stream = openFileWrite(path, format->mBinary);
        if (stream) {
            return Serialize::SerializeOutStream { std::make_unique<Serialize::WrappingSerializeStreambuf>(stream.release(), std::move(format), *this, createStreamId()) };           
        } else {
            return {};
        }        
    }

}
}
