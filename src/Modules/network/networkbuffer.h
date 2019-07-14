#pragma once

#include "../serialize/streams/buffered_streambuf.h"

#include "Interfaces/socket/socketapi.h"

namespace Engine
{
	namespace Network
	{
		class MODULES_EXPORT NetworkBuffer :
			public Serialize::buffered_streambuf
		{
		public:
			NetworkBuffer(SocketId socket, Serialize::SerializeManager &mgr, Serialize::ParticipantId id = 0);
			NetworkBuffer(const NetworkBuffer&) = delete;
			NetworkBuffer(NetworkBuffer&& other) noexcept;
			virtual ~NetworkBuffer();

		protected:
			// Geerbt über buffered_streambuf
			Serialize::StreamError getError() override;

			int recv(char*, size_t) override;

			int send(char*, size_t) override;

		private:
			SocketId mSocket; // = SOCKET
		};
	}
}
