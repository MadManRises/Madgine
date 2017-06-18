#include "interfaceslib.h"

#include "networkbuffer.h"

namespace Engine {
	namespace Network {

		NetworkBuffer::NetworkBuffer(SocketId socket) :
			mSocket(socket)			
		{
			
		}


		NetworkBuffer::NetworkBuffer(NetworkBuffer && other) :
			buffered_streambuf(std::forward<NetworkBuffer>(other)),
			mSocket(other.mSocket)
		{
			other.mSocket = Invalid_Socket;
		}

		NetworkBuffer::~NetworkBuffer()
		{
			if (mSocket != Invalid_Socket)
				SocketAPI::closeSocket(mSocket);
		}		

		size_t NetworkBuffer::rec(char *buf, size_t len)
		{
			return SocketAPI::recv(mSocket, buf, len);
		}

		size_t NetworkBuffer::send(char *buf, size_t len)
		{
			return SocketAPI::send(mSocket, buf, len);
		}

		Serialize::StreamError NetworkBuffer::getError()
		{
			return SocketAPI::getError();
		}	

	}
}
