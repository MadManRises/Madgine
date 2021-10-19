#pragma once

#include "Generic/bytebuffer.h"

namespace Engine {
namespace Memory {
    struct MADGINE_MEMORY_SERIALIZE_EXPORT MemoryBuffer : std::basic_streambuf<char> {
        MemoryBuffer(WritableByteBuffer buffer);
        MemoryBuffer(ByteBuffer buffer);
        MemoryBuffer(const MemoryBuffer &) = delete;
        MemoryBuffer(MemoryBuffer &&other) noexcept;
        virtual ~MemoryBuffer();

    protected:
        int_type overflow(int c = EOF) override;
        int_type underflow() override;

    private:
        WritableByteBuffer mWriteBuffer;
        ByteBuffer mReadBuffer;
    };
}
}
