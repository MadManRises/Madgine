#include "maditorsharedlib.h"

#include "boostipcbuffer.h"

#include "boostipcconnection.h"

#include "Madgine/serialize/serializemanager.h"

#undef min

namespace Maditor {
	namespace Shared {

		BoostIPCBuffer::BoostIPCBuffer(SharedConnectionPtr && conn, bool slave) :
			mConnection(std::forward<SharedConnectionPtr>(conn)),
			mReadQueue(boost::interprocess::open_only, (mConnection->prefix() + (slave ? "master" : "slave")).c_str()),
			mWriteQueue(boost::interprocess::open_only, (mConnection->prefix() + (slave ? "slave" : "master")).c_str()),
			mSlave(slave)			
		{
		}

		BoostIPCBuffer::BoostIPCBuffer(BoostIPCBuffer && other) :
			buffered_streambuf(std::forward<BoostIPCBuffer>(other)),
			mConnection(std::move(other.mConnection)),
			mReadQueue(boost::interprocess::open_only, (mConnection->prefix() + (other.mSlave ? "master" : "slave")).c_str()),
			mWriteQueue(boost::interprocess::open_only, (mConnection->prefix() + (other.mSlave ? "slave" : "master")).c_str()),
			mSlave(other.mSlave)
		{
			other.close();
		}

		BoostIPCBuffer::~BoostIPCBuffer()
		{
		}

		bool BoostIPCBuffer::isClosed()
		{
			return buffered_streambuf::isClosed();
		}

		Engine::Serialize::StreamError BoostIPCBuffer::getError()
		{
			switch (mError) {
			case WOULD_BLOCK:
				return Engine::Serialize::WOULD_BLOCK;
			default:
				return Engine::Serialize::UNKNOWN_ERROR;
			}
		}

		size_t BoostIPCBuffer::rec(char *buf, size_t len)
		{
			size_t receivedSize = 0;
			unsigned int priority;
			char buffer[sMaxMessageSize];
			if (mReadQueue.try_receive(buffer, sMaxMessageSize, receivedSize, priority)) {
				if (receivedSize > len) {
					mError = SIZE_MISMATCH;
					return -1;
				}
				std::memcpy(buf, buffer, receivedSize);
				return receivedSize;
			}
			else {
				if (mConnection.use_count() == 1) {
					Engine::Serialize::MessageHeader header;
					header.mObject = Engine::Serialize::SERIALIZE_MANAGER;
					header.mCmd = Engine::Serialize::STREAM_EOF;
					send(reinterpret_cast<char*>(&header), sizeof(header));
				}
					
				mError = WOULD_BLOCK;
				return -1;
			}

		}

		size_t BoostIPCBuffer::send(char *buf, size_t len)
		{
			size_t num = std::min(sMaxMessageSize, len);
			if (mWriteQueue.try_send(buf, num, 0))
				return num;
			else {
				if (mConnection.use_count() == 1)
					return 0;
				mError = WOULD_BLOCK;
				return -1;
			}
		}

		void BoostIPCBuffer::close(Engine::Serialize::StreamError error)
		{
			buffered_streambuf::close(error);
			mConnection.reset();
		}

	}
}
