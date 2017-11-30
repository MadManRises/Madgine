#pragma once

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
			static std::pair<SocketId, Serialize::StreamError> accept(SocketId s, int timeout = 0);
			static std::pair<SocketId, Serialize::StreamError> connect(const std::string& url, int portNr);

			static size_t send(SocketId id, char* buf, size_t len);
			static size_t recv(SocketId id, char* buf, size_t len);

			static Serialize::StreamError getError();
		};
	}
}
