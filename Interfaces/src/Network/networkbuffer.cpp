#include "interfaceslib.h"

#include "networkbuffer.h"

#include <WinSock2.h>
#include "networkexception.h"

namespace Engine {
	namespace Network {

		NetworkBuffer::NetworkBuffer(UINT_PTR socket) :
			mSocket(socket)			
		{

		}

		NetworkBuffer::~NetworkBuffer()
		{
		}

		bool NetworkBuffer::handleError()
		{
			int error = WSAGetLastError();
			switch (error) {
			case WSAEWOULDBLOCK:
				return true;
			default:
				close();
				return false;
			}
		}

		size_t NetworkBuffer::rec(char *buf, size_t len)
		{
			return recv(mSocket, buf, len, 0);
		}

		size_t NetworkBuffer::send(char *buf, size_t len)
		{
			return ::send(mSocket, buf, len, 0);
		}


		

		

	}
}
