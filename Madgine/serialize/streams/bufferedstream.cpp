#include "../../interfaceslib.h"

#include "bufferedstream.h"
#include "buffered_streambuf.h"

#include "../serializableunit.h"
#include "../serializemanager.h"

namespace Engine
{
	namespace Serialize
	{
		bool CompareStreamId::operator()(BufferedOutStream* first, BufferedOutStream* second) const
		{
			return first->id() < second->id();
		}


		BufferedInStream::BufferedInStream(buffered_streambuf& buffer, SerializeManager& mgr, ParticipantId id) :
			Stream(mgr, id),
			SerializeInStream(mIfs, mgr, id),
			mBuffer(buffer),
			mIfs(&buffer)
		{
		}

		bool BufferedInStream::isMessageAvailable()
		{
			return mBuffer.isMessageAvailable(mManager.name());
		}

		buffered_streambuf* BufferedInStream::rdbuf()
		{
			return &mBuffer;
		}

		void BufferedInStream::readHeader(MessageHeader& header)
		{
			if (!isMessageAvailable())
				throw 0;
			read(header);
		}

		BufferedOutStream::BufferedOutStream(buffered_streambuf& buffer, SerializeManager& mgr, ParticipantId id) :
			Stream(mgr, id),
			SerializeOutStream(mOfs, mgr, id),
			mBuffer(buffer),
			mOfs(&buffer)
		{
		}

		void BufferedOutStream::beginMessage(SerializableUnitBase* unit, MessageType type)
		{
			MessageHeader header;
			header.mType = type;
			header.mObject = mManager.convertPtr(*this, unit);
			mLog.logBeginSendMessage(header, typeid(*unit).name());
			mBuffer.beginMessage();
			write(header);
		}

		void BufferedOutStream::beginMessage(Command cmd)
		{
			MessageHeader header;
			header.mCmd = cmd;
			header.mObject = SERIALIZE_MANAGER;
			mLog.logBeginSendMessage(header, mManager.name());
			mBuffer.beginMessage();
			write(header);
		}

		void BufferedOutStream::endMessage()
		{
			mBuffer.endMessage();
		}

		int BufferedOutStream::sendMessages()
		{
			return mBuffer.sendMessages();
		}

		BufferedOutStream& BufferedOutStream::operator<<(BufferedInStream& in)
		{
			mOfs << in.rdbuf();
			return *this;
		}

		BufferedInOutStream::BufferedInOutStream(buffered_streambuf& buffer, SerializeManager& mgr, ParticipantId id) :
			Stream(mgr, id),
			BufferedOutStream(buffer, mgr, id),
			BufferedInStream(buffer, mgr, id),
			mBuffer(buffer)
		{
		}

		StreamError BufferedInOutStream::error() const
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
