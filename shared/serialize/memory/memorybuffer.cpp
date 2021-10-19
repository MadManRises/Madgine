#include "memorylib.h"

#include "memorybuffer.h"

namespace Engine {
namespace Memory {
    MemoryBuffer::MemoryBuffer(WritableByteBuffer buffer)
        : mWriteBuffer(std::move(buffer))
    {
    }

    MemoryBuffer::MemoryBuffer(ByteBuffer buffer)
        : mReadBuffer(std::move(buffer))
    {
        setg(
            const_cast<char *>(static_cast<const char *>(mReadBuffer.mData)),
            const_cast<char *>(static_cast<const char *>(mReadBuffer.mData)),
            const_cast<char *>(static_cast<const char *>(mReadBuffer.mData) + mReadBuffer.mSize));
    }

    MemoryBuffer::MemoryBuffer(MemoryBuffer &&other) noexcept
        : std::basic_streambuf<char>(std::move(other))
        , mWriteBuffer(std::move(other.mWriteBuffer))
        , mReadBuffer(std::move(other.mReadBuffer))
    {
    }

    MemoryBuffer::~MemoryBuffer()
    {
    }

    MemoryBuffer::int_type MemoryBuffer::overflow(int c)
    {
        size_t oldSize = mWriteBuffer.mSize;
        size_t newSize = 3 * oldSize / 2;
        if (newSize <= oldSize) {
            newSize = 16;
            assert(newSize > oldSize);
        }
        std::vector<char> newBuffer(newSize);
        std::memcpy(newBuffer.data(), mWriteBuffer.mData, oldSize);
        char *data = newBuffer.data();
        data[oldSize] = c;
        mWriteBuffer = std::move(newBuffer);
        setp(data + oldSize + 1, data + newSize);
        return c;
    }

    MemoryBuffer::int_type MemoryBuffer::underflow()
    {
        return traits_type::eof();
    }

}
}
