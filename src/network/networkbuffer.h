#pragma once

#include "../serialize/streams/buffered_streambuf.h"

#include "socketapi.h"

namespace Engine
{
	namespace Network
	{
		class INTERFACES_EXPORT NetworkBuffer :
			public Serialize::buffered_streambuf
		{
		public:
			NetworkBuffer(SocketId socket);
			NetworkBuffer(const NetworkBuffer&) = delete;
			NetworkBuffer(NetworkBuffer&& other) noexcept;
			virtual ~NetworkBuffer();

		protected:
			// Geerbt über buffered_streambuf
			Serialize::StreamError getError() override;

			size_t rec(char*, size_t) override;

			size_t send(char*, size_t) override;

		private:
			SocketId mSocket; // = SOCKET
		};
	}
}
