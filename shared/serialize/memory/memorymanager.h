#pragma once

#include "Meta/serialize/serializemanager.h"
#include "Generic/bytebuffer.h"

namespace Engine {
namespace Memory {

    
    struct MADGINE_MEMORY_SERIALIZE_EXPORT MemoryManager : Serialize::SerializeManager {
        MemoryManager(const std::string &name);
        MemoryManager(const MemoryManager &) = delete;
        MemoryManager(MemoryManager &&) noexcept;
        virtual ~MemoryManager();

        void operator=(const MemoryManager &) = delete;

        Serialize::SerializeInStream openRead(ByteBuffer buffer, std::unique_ptr<Serialize::Formatter> format);
        Serialize::SerializeOutStream openWrite(WritableByteBuffer buffer, std::unique_ptr<Serialize::Formatter> format);

    };
}
}
