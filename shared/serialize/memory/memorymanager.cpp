#include "memorylib.h"

#include "memorymanager.h"

#include "memorybuffer.h"

#include "Meta/serialize/streams/serializestream.h"
#include "Meta/serialize/streams/serializestreamdata.h"

#include "Meta/serialize/formatter.h"

#include "Meta/serialize/streams/formattedserializestream.h"

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

    Serialize::FormattedSerializeStream MemoryManager::openRead(ByteBuffer buffer, std::unique_ptr<Serialize::Formatter> format)
    {
        Serialize::FormattedSerializeStream stream { std::move(format), std::make_unique<MemoryReadBuffer>(std::move(buffer)), std::make_unique<Serialize::SerializeStreamData>(*this, createStreamId()) };
        setSlaveStreamData(&stream.data());
        return stream;
    }

    Serialize::FormattedSerializeStream MemoryManager::openWrite(WritableByteBuffer buffer, std::unique_ptr<Serialize::Formatter> format)
    {
        return { std::move(format), std::make_unique<MemoryWriteBuffer>(std::move(buffer)), std::make_unique<Serialize::SerializeStreamData>(*this, createStreamId()) };
    }

}
}
