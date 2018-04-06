#pragma once

#include "../serialize/streams/bufferedstream.h"
#include "networkbuffer.h"

namespace Engine
{
	namespace Network
	{
		class INTERFACES_EXPORT NetworkStream :
			public Serialize::BufferedInOutStream
		{
		public:
			NetworkStream(SocketId socket, Serialize::SerializeManager& mgr, Serialize::ParticipantId id = 0);
			NetworkStream(const NetworkStream&) = delete;
			NetworkStream(NetworkStream&& other, Serialize::SerializeManager& mgr);
			virtual ~NetworkStream();


		private:

			NetworkBuffer mBuffer;
		};
	}
}
