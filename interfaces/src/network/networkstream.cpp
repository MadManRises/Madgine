#include "interfaceslib.h"

#include "networkstream.h"

namespace Engine {
	namespace Network {

		NetworkStream::NetworkStream(SocketId socket, Serialize::SerializeManager &mgr, Serialize::ParticipantId id) :
			Stream(mgr, id),
			BufferedInOutStream(mBuffer, mgr, id),
			mBuffer(socket)			
		{
			
		}

		NetworkStream::NetworkStream(NetworkStream && other, Serialize::SerializeManager & mgr) :
			Stream(mgr, other.mId),
			BufferedInOutStream(mBuffer, mgr, other.mId),
			mBuffer(std::forward<NetworkBuffer>(other.mBuffer))
		{
			
		}

		NetworkStream::~NetworkStream()
		{
			
		}



		
	}
}
