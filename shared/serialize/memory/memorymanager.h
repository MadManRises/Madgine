#pragma once

#include "Meta/serialize/serializemanager.h"
#include "memorybuffer.h"

namespace Engine {
namespace Memory {

    
    struct MADGINE_MEMORY_EXPORT MemoryManager : Serialize::SerializeManager {
        MemoryManager(const std::string &name);
        MemoryManager(const MemoryManager &) = delete;
        MemoryManager(MemoryManager &&) noexcept;
        virtual ~MemoryManager();

        void operator=(const MemoryManager &) = delete;

        Serialize::SerializeInStream openRead(ByteBuffer buffer, std::unique_ptr<Serialize::Formatter> format);
        Serialize::SerializeOutStream openWrite(std::vector<char> &buffer, std::unique_ptr<Serialize::Formatter> format);

    };
}
}
