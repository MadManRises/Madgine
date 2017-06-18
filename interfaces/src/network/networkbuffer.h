#pragma once

#include "serialize/streams/buffered_streambuf.h"

#include "socketapi.h"

namespace Engine {
	namespace Network {


		class INTERFACES_EXPORT NetworkBuffer : 
			public Serialize::buffered_streambuf
		{

		public:
			NetworkBuffer(SocketId socket);
			NetworkBuffer(const NetworkBuffer &) = delete;
			NetworkBuffer(NetworkBuffer &&other);
			virtual ~NetworkBuffer();

		protected:
			// Geerbt über buffered_streambuf
			virtual Serialize::StreamError getError() override;

			virtual size_t rec(char *, size_t) override;

			virtual size_t send(char *, size_t) override;

		private:
			SocketId mSocket; // = SOCKET
			



		};

	}
}

