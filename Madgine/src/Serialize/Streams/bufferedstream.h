#pragma once

#include "serializestream.h"

namespace Engine {
namespace Serialize {

	
	class MADGINE_EXPORT BufferedInStream : public SerializeInStream {
public:
    BufferedInStream(buffered_streambuf &buffer, SerializeManager &mgr);

	bool isMessageAvailable();

private:
    std::istream mIfs;
	buffered_streambuf &mBuffer;
};

class MADGINE_EXPORT BufferedOutStream : public SerializeOutStream{
public:
    BufferedOutStream(buffered_streambuf &buffer, SerializeManager &mgr);

	void beginMessage();
	void endMessage();

private:
    std::ostream mOfs;
	buffered_streambuf &mBuffer;
};

class MADGINE_EXPORT BufferedInOutStream : 
	public BufferedOutStream,
	public BufferedInStream 
{
public:
	BufferedInOutStream(buffered_streambuf &buffer, SerializeManager &mgr);

	bool isValid();

	bool isBlocked();
	void setBlocked(bool blocked);

private:
	buffered_streambuf &mBuffer;

	bool mBlocked;
};


}
} // namespace Scripting

