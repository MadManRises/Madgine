#include "memorylib.h"

#include "memorybuffer.h"

#include "Meta/serialize/formatter.h"

namespace Engine {
namespace Memory {
    MemoryBuffer::MemoryBuffer(std::vector<char> &buffer, std::unique_ptr<Serialize::Formatter> format, Serialize::SerializeManager &mgr, Serialize::ParticipantId id)
        : SerializeStreambuf(std::move(format), mgr, id)
        , mWriteBuffer(&buffer)
    {
        //TODO: remove this later
        mWriteBuffer->clear();
    }

    MemoryBuffer::MemoryBuffer(ByteBuffer buffer, std::unique_ptr<Serialize::Formatter> format, Serialize::SerializeManager &mgr, Serialize::ParticipantId id)
        : SerializeStreambuf(std::move(format), mgr, id)
        , mReadBuffer(std::move(buffer))
    {
        setg(
            const_cast<char *>(static_cast<const char *>(mReadBuffer.mData)), 
            const_cast<char *>(static_cast<const char *>(mReadBuffer.mData)), 
            const_cast<char *>(static_cast<const char *>(mReadBuffer.mData) + mReadBuffer.mSize)
        );
    }

    MemoryBuffer::MemoryBuffer(MemoryBuffer &&other) noexcept
        : SerializeStreambuf(std::move(other))
        , mWriteBuffer(std::exchange(other.mWriteBuffer, nullptr))
        , mReadBuffer(std::move(other.mReadBuffer))
    {
    }

    MemoryBuffer::~MemoryBuffer()
    {
    }

    MemoryBuffer::int_type MemoryBuffer::overflow(int c)
    {
        size_t oldSize = mWriteBuffer->size();
        size_t newSize = 3 * oldSize / 2;
        if (newSize <= oldSize) {
            newSize = 16;
            assert(newSize > oldSize);
        }
        mWriteBuffer->resize(newSize);
        char *data = mWriteBuffer->data();
        data[oldSize] = c;
        setp(data + oldSize + 1, data + newSize);
        return c;
    }

    MemoryBuffer::int_type MemoryBuffer::underflow()
    {
        return traits_type::eof();
    }

}
}
