#include "memorylib.h"

#include "memorymanager.h"

#include "memorybuffer.h"

#include "Meta/serialize/streams/serializestream.h"
#include "Meta/serialize/streams/serializestreamdata.h"

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

    Serialize::SerializeOutStream MemoryManager::openWrite(WritableByteBuffer buffer, std::unique_ptr<Serialize::Formatter> format)
    {
        return { std::make_unique<MemoryBuffer>(std::move(buffer)), std::make_unique<Serialize::SerializeStreamData>(std::move(format), *this, createStreamId()) };
    } 

}
}
