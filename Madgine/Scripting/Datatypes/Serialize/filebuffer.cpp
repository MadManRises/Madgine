#include "libinclude.h"
#include "filebuffer.h"

#include "serializestream.h"


namespace Engine {
namespace Scripting {
namespace Serialize {


FileBuffer::FileBuffer()
{

}

void FileBuffer::save(SerializeOutStream &out) const
{
    out << size();
    out.write(data(), size());
}

void FileBuffer::load(SerializeInStream &in)
{
    size_t size;
    in >> size;
    clear();
    resize(size);
    if (size > 0)
        in.read(data(), size);
}


FileBufferReader::FileBufferReader(FileBuffer &buffer) :
    mBuffer(buffer)
{
    setg(mBuffer.data(), mBuffer.data(), mBuffer.data() + mBuffer.size());
}

FileBufferWriter::FileBufferWriter(FileBuffer &buffer) :
    mBuffer(buffer)
{
    extend();
}

FileBufferWriter::~FileBufferWriter()
{
    sync();
}

void FileBufferWriter::extend()
{
    mDataBuffer.emplace_back();
    setp(mDataBuffer.back().data(), mDataBuffer.back().data(), mDataBuffer.back().data() + BUFFER_SIZE);
}

FileBufferWriter::int_type FileBufferWriter::overflow(int c)
{
    if (c != EOF){
        extend();
        *pptr() = c;
        pbump(1);
        return c;
    }
    return traits_type::eof();
}

int FileBufferWriter::sync()
{
    mBuffer.resize(bufferByteSize());
    auto end = mDataBuffer.end();
    --end;
    char *buffer = mBuffer.data();
    for (auto it = mDataBuffer.begin(); it != end; ++it){
        std::memcpy(buffer, it->data(), BUFFER_SIZE);
        buffer += BUFFER_SIZE;
    }
    std::memcpy(buffer, pbase(), pptr() - pbase());
    mDataBuffer.clear();
    extend();
    return 0;
}

size_t FileBufferWriter::bufferByteSize() const
{
    return (mDataBuffer.size() - 1) * BUFFER_SIZE + (pptr() - pbase());
}



} // namespace Serialize
} // namespace Scripting
} // namespace Core

