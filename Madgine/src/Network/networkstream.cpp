#include "madginelib.h"

#include "networkstream.h"

#include <WinSock2.h>

namespace Engine {
	namespace Network {

		NetworkStream::NetworkStream(UINT_PTR socket, Serialize::SerializeManager &mgr) :
			mSocket(socket),
			mBuffer(socket),
			mStream(&mBuffer),
			SerializeOutStream(mStream, mgr),
			SerializeInStream(mStream, mgr)
		{
			u_long iMode = 1;
			ioctlsocket(mSocket, FIONBIO, &iMode);
		}

		NetworkStream::~NetworkStream()
		{
			closesocket(mSocket);
		}

		bool NetworkStream::isMessageAvailable()
		{
			return mBuffer.isMessageAvailable();
		}


		
	}
}
