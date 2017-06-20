#pragma once

#include "serializestream.h"
#include "../serializemanager.h"

namespace Engine {
namespace Serialize {

	
	class INTERFACES_EXPORT BufferedInStream : public SerializeInStream {
public:
    BufferedInStream(buffered_streambuf &buffer, SerializeManager &mgr, ParticipantId id);

	bool isMessageAvailable();

	buffered_streambuf *rdbuf();

private:
	buffered_streambuf &mBuffer;
	std::istream mIfs;
};

class INTERFACES_EXPORT BufferedOutStream : public SerializeOutStream{
public:
    BufferedOutStream(buffered_streambuf &buffer, SerializeManager &mgr, ParticipantId id);

	void beginMessage();
	void endMessage();

	void sendMessages();

	BufferedOutStream &operator<< (BufferedInStream &in);
	using SerializeOutStream::operator<<;

	template <class... _Ty>
	void writeCommand(Command cmd, const _Ty&... args)
	{
		MessageHeader header;
		header.mObject = SERIALIZE_MANAGER;
		header.mCmd = cmd;
		beginMessage();
		write(header);

		using expander = bool[];
		(void)expander {
			false,
			(*this << args, false)...
		};

		endMessage();
	}	

private:
	buffered_streambuf &mBuffer;
	std::ostream mOfs;	
};

class INTERFACES_EXPORT BufferedInOutStream : 
	public BufferedOutStream,
	public BufferedInStream 
{
public:
	BufferedInOutStream(buffered_streambuf &buffer, SerializeManager &mgr, ParticipantId id);

	StreamError error();
	bool isClosed();
	void close();

	explicit operator bool();

private:
	buffered_streambuf &mBuffer;

};


}
} // namespace Scripting

