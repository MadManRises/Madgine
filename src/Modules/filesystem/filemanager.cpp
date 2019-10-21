#include "../moduleslib.h"

#include "filemanager.h"

#include "Interfaces/filesystem/path.h"

#include "Interfaces/filesystem/api.h"

#include "../serialize/streams/wrappingserializestreambuf.h"

namespace Engine {
namespace Filesystem {

    FileManager::FileManager(const std::string &name)
        : SerializeManager(name)
    {
    }

    std::optional<Serialize::SerializeInStream> FileManager::openRead(const Path &path, std::unique_ptr<Serialize::Formatter> format)
    {
        assert(!getSlaveStreambuf());

		InStream stream = openFile(path);

        Serialize::SerializeInStream in { std::make_unique<Serialize::WrappingSerializeStreambuf>(stream.release(), std::move(format), *this, createStreamId()) };
        setSlaveStreambuf(&in.buffer());

        return in;
    }

    std::optional<Serialize::SerializeOutStream> FileManager::openWrite(const Path &path, std::unique_ptr<Serialize::Formatter> format)
    {
        return {};
    }

}
}
