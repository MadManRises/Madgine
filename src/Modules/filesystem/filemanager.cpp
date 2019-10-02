#include "../moduleslib.h"

#include "filemanager.h"

#include "Interfaces/filesystem/path.h"

#include "Modules/serialize/streams/buffered_streambuf.h"

namespace Engine {
namespace Filesystem {

    FileManager::FileManager(const std::string &name)
        : SerializeManager(name)
    {
    }

    std::optional<Serialize::SerializeInStream> FileManager::openRead(const Path &path, std::unique_ptr<Serialize::Formatter> format)
    {
        assert(!getSlaveStreambuf());

        auto buf = std::make_unique<Serialize::WrappingSerializeStreambuf<std::filebuf>>(std::move(format), *this, createStreamId());
        if (!buf->open(path.str(), std::ios::in))
            return {};

        Serialize::SerializeInStream in { std::move(buf) };
        setSlaveStreambuf(&in.buffer());

        return in;
    }

    std::optional<Serialize::SerializeOutStream> FileManager::openWrite(const Path &path, std::unique_ptr<Serialize::Formatter> format)
    {
        return {};
    }

}
}
