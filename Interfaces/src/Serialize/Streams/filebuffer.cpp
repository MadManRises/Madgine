#include "interfaceslib.h"
#include "filebuffer.h"

#include "serializestream.h"


namespace Engine {
namespace Serialize {

void FileBuffer::writeState(SerializeOutStream &out) const
{
    out << size();
    out.writeData(data(), size());
}

void FileBuffer::readState(SerializeInStream &in)
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

pos_type FileBufferReader::seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode mode)
{
	if (mode & std::ios_base::in) {
		switch (dir) {
		case std::ios_base::beg:
			if (0 <= off && eback() + off <= egptr()) {
				setg(eback(), eback() + off, egptr());
				return pos_type(off);
			}
			break;
		case std::ios_base::cur:
			if (eback() <= gptr() + off && gptr() + off <= egptr()) {
				setg(eback(), gptr() + off, egptr());
				return pos_type(gptr() - eback());
			}
			break;
		case std::ios_base::end:
			if (eback() <= egptr() + off && off <= 0) {
				setg(eback(), egptr() + off, egptr());
				return pos_type(gptr() - eback());
			}
		}
	}

	return pos_type(off_type(-1));
	
}

pos_type FileBufferReader::seekpos(pos_type pos, std::ios_base::openmode mode)
{
	if (0 <= pos && eback() + pos <= egptr()) {
		setg(eback(), eback() + pos, egptr());
		return pos;
	}
	return pos_type(off_type(-1));
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
    setp(mDataBuffer.back().data(), mDataBuffer.back().data() + BUFFER_SIZE);
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
} // namespace Core

