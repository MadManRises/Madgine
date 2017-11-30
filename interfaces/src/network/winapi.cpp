#include "interfaceslib.h"

#ifdef _WIN32

#include "socketapi.h"

#include <WinSock2.h>
#include <WS2tcpip.h>

#undef NO_ERROR

namespace Engine
{
	namespace Network
	{
		bool SocketAPI::init()
		{
			WSADATA w;

			int error = WSAStartup(MAKEWORD(2, 2), &w);

			if (error)
			{
				return false;
			}

			if (w.wVersion != MAKEWORD(2, 2))
			{
				WSACleanup();
				return false;
			}
			return true;
		}

		void SocketAPI::finalize()
		{
			WSACleanup();
		}

		void SocketAPI::closeSocket(SocketId id)
		{
			closesocket(id);
		}

		size_t SocketAPI::send(SocketId id, char* buf, size_t len)
		{
			return ::send(id, buf, len, 0);
		}

		size_t SocketAPI::recv(SocketId id, char* buf, size_t len)
		{
			return ::recv(id, buf, len, 0);
		}

		Serialize::StreamError SocketAPI::getError()
		{
			int error = WSAGetLastError();
			switch (error)
			{
			case WSAEWOULDBLOCK:
				return Serialize::WOULD_BLOCK;
			default:
				return Serialize::UNKNOWN_ERROR;
			}
		}

		SocketId SocketAPI::socket(int port)
		{
			SOCKADDR_IN addr;
			memset(&addr, 0, sizeof(addr));
			addr.sin_family = AF_INET;
			addr.sin_port = htons(port);

			addr.sin_addr.s_addr = htonl(INADDR_ANY);

			SocketId s = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			if (s == INVALID_SOCKET)
			{
				return Invalid_Socket;
			}

			if (bind(s, LPSOCKADDR(&addr), sizeof(addr)) == SOCKET_ERROR)
			{
				closesocket(s);
				return Invalid_Socket;
			}

			int result = listen(s, SOMAXCONN);
			if (result == SOCKET_ERROR)
			{
				closesocket(s);
				return Invalid_Socket;
			}
			return s;
		}

		std::pair<SocketId, Serialize::StreamError> SocketAPI::accept(SocketId s, int timeout)
		{
			fd_set readSet;
			FD_ZERO(&readSet);
			FD_SET(s, &readSet);
			timeval timeout_s;
			timeout_s.tv_sec = timeout / 1000;
			timeout_s.tv_usec = (timeout % 1000) * 1000;
			if (select(s, &readSet, nullptr, nullptr, &timeout_s) > 0)
			{
				SocketId sock = ::accept(s, nullptr, nullptr);
				u_long iMode = 1;
				if (ioctlsocket(sock, FIONBIO, &iMode))
				{
					closesocket(sock);
					return {Invalid_Socket, Serialize::UNKNOWN_ERROR};
				}
				return {sock, Serialize::NO_ERROR};
			}
			return {Invalid_Socket, Serialize::TIMEOUT};
		}

		std::pair<SocketId, Serialize::StreamError> SocketAPI::connect(const std::string& url, int portNr)
		{
			//Fill out the information needed to initialize a socket…
			SOCKADDR_IN target; //Socket address information

			target.sin_family = AF_INET; // address family Internet
			target.sin_port = htons(portNr); //Port to connect on
			target.sin_addr.s_addr = inet_addr(url.c_str());

			SocketId s = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //Create socket

			if (s == INVALID_SOCKET)
			{
				return {Invalid_Socket, getError()};
			}

			//Try connecting...

			if (::connect(s, reinterpret_cast<SOCKADDR *>(&target), sizeof(target)) == SOCKET_ERROR)
			{
				Serialize::StreamError error = getError();
				closesocket(s);
				return {Invalid_Socket, error};
			}

			u_long iMode = 1;
			if (ioctlsocket(s, FIONBIO, &iMode))
			{
				Serialize::StreamError error = getError();
				closesocket(s);
				return {Invalid_Socket, error};
			}

			return {s, Serialize::NO_ERROR};
		}
	}
}

#endif
