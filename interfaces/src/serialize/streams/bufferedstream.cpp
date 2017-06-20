#include "interfaceslib.h"

#include "bufferedstream.h"
#include "buffered_streambuf.h"

namespace Engine {
namespace Serialize {



	BufferedInStream::BufferedInStream(buffered_streambuf & buffer, SerializeManager & mgr, ParticipantId id) :
		Stream(mgr, id),
		SerializeInStream(mIfs, mgr, id),
		mBuffer(buffer),
		mIfs(&buffer)
	{
	}

	bool BufferedInStream::isMessageAvailable()
	{
		return mBuffer.isMessageAvailable();
	}

	buffered_streambuf * BufferedInStream::rdbuf()
	{
		return &mBuffer;
	}

	BufferedOutStream::BufferedOutStream(buffered_streambuf & buffer, SerializeManager & mgr, ParticipantId id) :
		Stream(mgr, id),
		SerializeOutStream(mOfs, mgr, id),		
		mBuffer(buffer),
		mOfs(&buffer)		
	{
	}

	void BufferedOutStream::beginMessage()
	{
		mBuffer.beginMessage();
	}

	void BufferedOutStream::endMessage()
	{
		mBuffer.endMessage();
	}

	void BufferedOutStream::sendMessages()
	{
		mBuffer.sendMessages();
	}

	BufferedOutStream & BufferedOutStream::operator<<(BufferedInStream & in)
	{
		mOfs << in.rdbuf();
		return *this;
	}

	BufferedInOutStream::BufferedInOutStream(buffered_streambuf & buffer, SerializeManager & mgr, ParticipantId id) :
		Stream(mgr, id),
		BufferedOutStream(buffer, mgr, id),
		BufferedInStream(buffer, mgr, id),
		mBuffer(buffer)
	{
	}

	StreamError Engine::Serialize::BufferedInOutStream::error()
	{
		return mBuffer.closeCause();
	}

	bool BufferedInOutStream::isClosed()
	{
		return !bool(*this) || mBuffer.isClosed();
	}
	void BufferedInOutStream::close()
	{
		writeCommand(STREAM_EOF);
		mBuffer.close();
	}
	BufferedInOutStream::operator bool()
	{
		return this->SerializeInStream::operator bool() && this->SerializeOutStream::operator bool();
	}
}
} // namespace Scripting

