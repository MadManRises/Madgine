#include "../../metalib.h"

#include "wrappingserializestreambuf.h"

#include "../formatter.h"

namespace Engine {
namespace Serialize {



WrappingSerializeStreambuf::WrappingSerializeStreambuf(std::unique_ptr<std::streambuf> buffer, std::unique_ptr<Formatter> format)
        : SerializeStreambuf(std::move(format))
        , mBuffer(std::move(buffer))
    {
}

WrappingSerializeStreambuf::WrappingSerializeStreambuf(std::unique_ptr<std::streambuf> buffer, std::unique_ptr<Formatter> format, SerializeManager &mgr, ParticipantId id)
    : SerializeStreambuf(std::move(format), mgr, id)
    , mBuffer(std::move(buffer))
{
}

void WrappingSerializeStreambuf::imbue(const std::locale &loc)
{
    mBuffer->pubimbue(loc);
}

WrappingSerializeStreambuf::pos_type WrappingSerializeStreambuf::seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode mode)
{
    return mBuffer->pubseekoff(off, dir, mode);
}

WrappingSerializeStreambuf::pos_type WrappingSerializeStreambuf::seekpos(pos_type pos, std::ios_base::openmode mode)
{
    return mBuffer->pubseekpos(pos, mode);
}

std::streamsize WrappingSerializeStreambuf::showmanyc()
{
    return mBuffer->in_avail();
}

std::streamsize WrappingSerializeStreambuf::xsgetn(char *s, std::streamsize n)
{
    return mBuffer->sgetn(s, n);
}

int WrappingSerializeStreambuf::pbackfail(int c)
{
    return mBuffer->sputbackc(c);
}

WrappingSerializeStreambuf::int_type WrappingSerializeStreambuf::overflow(int c)
{
    return mBuffer->sputc(c);
}

std::streamsize WrappingSerializeStreambuf::xsputn(const char *s, std::streamsize n)
{
    return mBuffer->sputn(s, n);
}

WrappingSerializeStreambuf::int_type WrappingSerializeStreambuf::underflow()
{
    return mBuffer->sgetc();
}

int WrappingSerializeStreambuf::uflow()
{
    return mBuffer->sbumpc();
}

int WrappingSerializeStreambuf::sync()
{
    return mBuffer->pubsync();
}

}
}