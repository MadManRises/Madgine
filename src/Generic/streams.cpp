#include "genericlib.h"

#include "streams.h"

namespace Engine {

InStream::InStream()
    : mStream(nullptr)
{
}

InStream::InStream(std::unique_ptr<std::streambuf> &&buffer)
    : mStream(buffer.release())
{
}

InStream::InStream(InStream &&other)
    : mStream(other.mStream.rdbuf(nullptr))
    , mOwning(std::exchange(other.mOwning, false))
{
}

InStream::~InStream()
{
    if (mOwning && mStream.rdbuf())
        delete mStream.rdbuf();
}

std::istreambuf_iterator<char> InStream::iterator()
{
    return std::istreambuf_iterator<char>(mStream);
}

std::istreambuf_iterator<char> InStream::end()
{
    return std::istreambuf_iterator<char>();
}

size_t InStream::readRaw(void *buffer, size_t size)
{
    mStream.read(static_cast<char *>(buffer), size);
    return mStream.gcount();
}

pos_type InStream::tell()
{
    return mStream.tellg();
}

bool InStream::seek(pos_type p)
{
    return static_cast<bool>(mStream.seekg(p));
}

bool InStream::seek(off_type p, std::ios::seekdir dir)
{
    return static_cast<bool>(mStream.seekg(p, dir));
}

void InStream::skipWs()
{
    if (mStream.flags() & std::ios_base::skipws) {
        mStream >> std::ws;
    }
}

InStream::operator bool() const
{
    return mStream.rdbuf() != nullptr && static_cast<bool>(mStream);
}

InStream::InStream(std::streambuf *buffer)
    : mStream(buffer)
    , mOwning(false)
{
}

std::streambuf &InStream::buffer() const
{
    return *mStream.rdbuf();
}

std::unique_ptr<std::streambuf> InStream::release()
{
    assert(mOwning);
    return std::unique_ptr<std::streambuf>(mStream.rdbuf(nullptr));
}

OutStream::OutStream()
    : mStream(nullptr)
{
}

OutStream::OutStream(std::unique_ptr<std::streambuf> &&buffer)
    : mStream(buffer.release())
{
}

OutStream::OutStream(OutStream &&other)
    : mStream(other.mStream.rdbuf(nullptr))
{
}

OutStream::~OutStream()
{
    if (mStream.rdbuf())
        delete mStream.rdbuf();
}

void OutStream::writeRaw(const void *data, size_t count)
{
    mStream.write(static_cast<const char *>(data), count);
}

std::streambuf &OutStream::buffer() const
{
    return *mStream.rdbuf();
}

void OutStream::pipe(InStream& in) 
{
    mStream << &in.buffer();
}

OutStream::operator bool() const
{
    return mStream.rdbuf() != nullptr && static_cast<bool>(mStream);
}

std::unique_ptr<std::streambuf> OutStream::release()
{
    return std::unique_ptr<std::streambuf>(mStream.rdbuf(nullptr));
}

Stream::Stream(std::unique_ptr<std::streambuf> &&buffer)
    : InStream(buffer.get())
    , OutStream(std::move(buffer))
{
}

}