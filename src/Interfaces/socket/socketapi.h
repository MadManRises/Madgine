#pragma once

#include "../timeout.h"

namespace Engine
{
	namespace Network
	{
		using SocketId = unsigned long long;
		const constexpr SocketId Invalid_Socket = -1;

		class INTERFACES_EXPORT SocketAPI
		{
		public:
			static bool init();
			static void finalize();

			static void closeSocket(SocketId id);
			static SocketId socket(int port);
			static std::pair<SocketId, StreamError> accept(SocketId s, TimeOut timeout = {});
			static std::pair<SocketId, StreamError> connect(const std::string& url, int portNr);

			static int send(SocketId id, char* buf, size_t len);
			static int recv(SocketId id, char* buf, size_t len);

			static StreamError getError();
			static int getAPIError();
			static std::string getErrorMsg();
		};
	}
}
