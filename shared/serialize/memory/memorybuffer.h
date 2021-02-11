#pragma once

#include "Meta/serialize/streams/serializestreambuf.h"

#include "Generic/bytebuffer.h"

namespace Engine {
namespace Memory {
    struct MADGINE_MEMORY_EXPORT MemoryBuffer : Serialize::SerializeStreambuf {
        MemoryBuffer(std::vector<char> &buffer, std::unique_ptr<Serialize::Formatter> format, Serialize::SerializeManager &mgr, Serialize::ParticipantId id = 0);
        MemoryBuffer(ByteBuffer buffer, std::unique_ptr<Serialize::Formatter> format, Serialize::SerializeManager &mgr, Serialize::ParticipantId id = 0);
        MemoryBuffer(const MemoryBuffer &) = delete;
        MemoryBuffer(MemoryBuffer &&other) noexcept;
        virtual ~MemoryBuffer();

    protected:
        int_type overflow(int c = EOF) override;
        int_type underflow() override;

    private:
        std::vector<char> *mWriteBuffer = nullptr;
        ByteBuffer mReadBuffer;
    };
}
}
