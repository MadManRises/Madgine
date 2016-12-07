#include "madginelib.h"

#include "networkbuffer.h"

#include <WinSock2.h>
#include "networkexception.h"

namespace Engine {
	namespace Network {

		NetworkBuffer::NetworkBuffer(UINT_PTR socket) :
			mSocket(socket),
			mMsgInBuffer(false),
			mBytesToRead(sizeof(NetworkMessageHeader))
		{
			extend();
		}

		NetworkBuffer::~NetworkBuffer()
		{
		}

		bool NetworkBuffer::isMessageAvailable()
		{
			if (mMsgInBuffer && mBytesToRead == 0 && gptr() == eback())
				return true;
			underflow();
			return (mMsgInBuffer && mBytesToRead == 0);
		}

		void NetworkBuffer::beginMessage()
		{
			if (mSendBuffer.size() != 1 || pptr() != pbase())
				throw 0;
		}

		void NetworkBuffer::sendMessage()
		{
			int sendBufferIndex = pptr() - pbase();
			mSendMessageHeader.mMsgSize = (mSendBuffer.size() - 1) * BUFFER_SIZE + sendBufferIndex;
			if (send(mSocket, reinterpret_cast<char*>(&mSendMessageHeader), sizeof(mSendMessageHeader), 0) != sizeof(mSendMessageHeader)) {
				throw 0;
			}			
			auto it = mSendBuffer.begin();
			while (mSendMessageHeader.mMsgSize > 0) {
				if (it == mSendBuffer.end())
					throw 0;
				size_t count = std::min(mSendMessageHeader.mMsgSize, BUFFER_SIZE);
				if (send(mSocket, it->data(), count, 0) != count) {
					throw 0;
				}
				mSendMessageHeader.mMsgSize -= count;
				it = mSendBuffer.erase(it);
			}
			if (mSendBuffer.empty()) {
				extend();
			}
			else {
				if (sendBufferIndex != 0 || mSendBuffer.size() != 1)
					throw 0;
			}
		}

		void NetworkBuffer::extend()
		{
			mSendBuffer.emplace_back();
			setp(mSendBuffer.back().data(), mSendBuffer.back().data() + BUFFER_SIZE);
		}

		NetworkBuffer::int_type NetworkBuffer::overflow(int c)
		{
			if (c != EOF) {
				extend();
				*pptr() = c;
				pbump(1);
				return c;
			}
			return traits_type::eof();
		}


		NetworkBuffer::int_type NetworkBuffer::underflow()
		{
			if (mMsgInBuffer && mBytesToRead == 0 && gptr() == egptr()) {
				delete[] mMsgBuffer;
				mMsgInBuffer = false;
				mBytesToRead = sizeof(mReceiveMessageHeader);
			}
			if (!mMsgInBuffer) {
				assert(mBytesToRead > 0);
				int num = recv(mSocket, reinterpret_cast<char*>(&mReceiveMessageHeader + 1) - mBytesToRead, mBytesToRead, 0);
				if (num == 0){
					return traits_type::eof();
				}
				if (num == SOCKET_ERROR) {
					int error = WSAGetLastError();
					switch (error) {
					case WSAEWOULDBLOCK:
						return traits_type::eof();
					default:
						throw NetworkException(error);
					}
				}
				mBytesToRead -= num;
				if (mBytesToRead == 0) {
					mMsgInBuffer = true;
					mBytesToRead = mReceiveMessageHeader.mMsgSize;
					mMsgBuffer = new char[mBytesToRead];
					std::cout << "Header received!" << std::endl;
				}
				else {
					return traits_type::eof();
				}

			}

			if (mMsgInBuffer && mBytesToRead > 0) {
				int num = recv(mSocket, (mMsgBuffer + mReceiveMessageHeader.mMsgSize - mBytesToRead), mBytesToRead, 0);
				if (num == 0){
					return traits_type::eof();
				}
				if (num == SOCKET_ERROR) {
					throw NetworkException(WSAGetLastError());
				}
				mBytesToRead -= num;
				if (mBytesToRead == 0) {
					setg(mMsgBuffer, mMsgBuffer, mMsgBuffer + mReceiveMessageHeader.mMsgSize);
					return *gptr();
				}
				else {
					return traits_type::eof();
				}				
			}
			
			throw 0;

		}

	}
}
