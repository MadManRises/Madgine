#pragma once

#include "serializestream.h"

namespace Engine {
namespace Serialize {

	
	class INTERFACES_EXPORT BufferedInStream : public SerializeInStream {
public:
    BufferedInStream(buffered_streambuf &buffer, SerializeManager &mgr);

	bool isMessageAvailable();

	buffered_streambuf *rdbuf();

private:
    std::istream mIfs;
	buffered_streambuf &mBuffer;
};

class INTERFACES_EXPORT BufferedOutStream : public SerializeOutStream{
public:
    BufferedOutStream(buffered_streambuf &buffer, SerializeManager &mgr);

	void beginMessage();
	void endMessage();

	BufferedOutStream &operator<< (BufferedInStream &in);
	using SerializeOutStream::operator<<;

private:
    std::ostream mOfs;
	buffered_streambuf &mBuffer;
};

class INTERFACES_EXPORT BufferedInOutStream : 
	public BufferedOutStream,
	public BufferedInStream 
{
public:
	BufferedInOutStream(buffered_streambuf &buffer, SerializeManager &mgr);

	bool isValid();

private:
	buffered_streambuf &mBuffer;

};


}
} // namespace Scripting

