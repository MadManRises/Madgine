#include "../moduleslib.h"

#include "networkbuffer.h"

namespace Engine
{
	namespace Network
	{
		NetworkBuffer::NetworkBuffer(SocketId socket, Serialize::SerializeManager &mgr, Serialize::ParticipantId id) :
			buffered_streambuf(mgr, id),
			mSocket(socket)
		{
		}


		NetworkBuffer::NetworkBuffer(NetworkBuffer&& other) noexcept :
			buffered_streambuf(std::move(other)),
			mSocket(std::exchange(other.mSocket, Invalid_Socket))
		{
		}

		NetworkBuffer::~NetworkBuffer()
		{
			if (mSocket != Invalid_Socket)
				SocketAPI::closeSocket(mSocket);
		}

		int NetworkBuffer::recv(char* buf, size_t len)
		{
			return SocketAPI::recv(mSocket, buf, len);
		}

		int NetworkBuffer::send(char* buf, size_t len)
		{
			return SocketAPI::send(mSocket, buf, len);
		}

		Serialize::StreamError NetworkBuffer::getError()
		{
			return SocketAPI::getError();
		}
	}
}
