#pragma once

#include "Meta/serialize/streams/serializestreambuf.h"

#include "Generic/bytebuffer.h"

namespace Engine {
namespace Memory {
    struct MADGINE_MEMORY_EXPORT MemoryBuffer : std::basic_streambuf<char> {
        MemoryBuffer(std::vector<char> &buffer);
        MemoryBuffer(ByteBuffer buffer);
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
