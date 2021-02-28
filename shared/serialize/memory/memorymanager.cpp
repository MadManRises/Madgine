#include "memorylib.h"

#include "memorymanager.h"

#include "Meta/serialize/formatter/safebinaryformatter.h"

#include "Meta/serialize/streams/serializestream.h"

#include "Meta/serialize/formatter.h"

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
        Serialize::SerializeInStream stream { std::make_unique<MemoryBuffer>(std::move(buffer)), std::make_unique<Serialize::SerializeStreamData>(std::move(format), *this, createStreamId()) };
        setSlaveStreamData(&stream.data());
        return stream;
    }

    Serialize::SerializeOutStream MemoryManager::openWrite(std::vector<char> &buffer, std::unique_ptr<Serialize::Formatter> format)
    {
        return { std::make_unique<MemoryBuffer>(buffer), std::make_unique<Serialize::SerializeStreamData>(std::move(format), *this, createStreamId()) };
    } 

}
}
