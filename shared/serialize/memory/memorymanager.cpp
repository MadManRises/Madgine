#include "memorylib.h"

#include "memorymanager.h"

#include "Modules/serialize/formatter/safebinaryformatter.h"

#include "Modules/threading/workgroup.h"

#include "Modules/serialize/streams/serializestream.h"

#include "Modules/serialize/formatter.h"

namespace Engine {
namespace Memory {

    MemoryManager::MemoryManager(const std::string &name)
        : SerializeManager(name)
    {
    }

    MemoryManager::MemoryManager(MemoryManager &&other) noexcept
        : SerializeManager(std::forward<MemoryManager>(other))
    {
    }

    MemoryManager::~MemoryManager()
    {
    }

    Serialize::SerializeInStream MemoryManager::openRead(ByteBuffer buffer, std::unique_ptr<Serialize::Formatter> format)
    {
        Serialize::SerializeInStream stream { std::make_unique<MemoryBuffer>(std::move(buffer), std::move(format), *this, createStreamId()) };
        setSlaveStreambuf(&stream.buffer());
        return stream;
    }

    Serialize::SerializeOutStream MemoryManager::openWrite(std::vector<char> &buffer, std::unique_ptr<Serialize::Formatter> format)
    {
        return { std::make_unique<MemoryBuffer>(buffer, std::move(format), *this, createStreamId()) };
    } 

}
}
