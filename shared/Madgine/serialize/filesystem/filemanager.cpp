#include "filesystemlib.h"

#include "filemanager.h"

#include "Interfaces/filesystem/path.h"

#include "Interfaces/filesystem/fsapi.h"

#include "Meta/serialize/streams/serializestream.h"

#include "Meta/serialize/streams/formatter.h"

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

        Stream stream = openFileRead(path, format->mBinary);
        if (stream) {
            return Serialize::FormattedSerializeStream {
                std::move(format),
                wrapStream(std::move(stream), true)
            };
        } else {
            return {};
        }
    }

    Serialize::FormattedSerializeStream FileManager::openWrite(const Path &path, std::unique_ptr<Serialize::Formatter> format)
    {
        Stream stream = openFileWrite(path, format->mBinary);
        if (stream) {
            return Serialize::FormattedSerializeStream {
                std::move(format),
                wrapStream(std::move(stream))
            };
        } else {
            return {};
        }
    }

}
}
