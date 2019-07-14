#include "../interfaceslib.h"

#if UNIX

#include "socketapi.h"

#include <sys/socket.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <arpa/inet.h>

namespace Engine {
	namespace Network {

		bool SocketAPI::init()
		{
			return true;
		}

		void SocketAPI::finalize()
		{
			
		}

		void SocketAPI::closeSocket(SocketId id)
		{
			close(id);
		}

		int SocketAPI::send(SocketId id, char *buf, size_t len)
		{
			return ::send(id, buf, len, MSG_NOSIGNAL);
		}

		int SocketAPI::recv(SocketId id, char *buf, size_t len)
		{
			return ::read(id, buf, len);
		}

		Serialize::StreamError SocketAPI::getError() {
			int error = errno;
			switch (error) {
			case EWOULDBLOCK:
			//case EAGAIN:
				return Serialize::WOULD_BLOCK;
			default:
				return Serialize::UNKNOWN_ERROR;
			}
		}

		int SocketAPI::getAPIError()
		{
			return errno;
		}

		Serialize::StreamError preInitSock(SocketId s)
		{
			int on = 1;
			if (setsockopt(s, SOL_TCP, TCP_NODELAY, &on, sizeof(on))  < 0)
			{
				return Serialize::UNKNOWN_ERROR;
			}
			if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
			{
				return Serialize::UNKNOWN_ERROR;
			}
			
			return Serialize::NO_ERROR;
		}
		
		Serialize::StreamError postInitSock(SocketId s){
			int flags = fcntl(s, F_GETFL, 0);
			if (flags < 0) {
				return Serialize::UNKNOWN_ERROR;
			}
			flags |= O_NONBLOCK;
			if (fcntl(s, F_SETFL, flags) != 0) {
				return Serialize::UNKNOWN_ERROR;
			}
			return Serialize::NO_ERROR;
		}

		SocketId SocketAPI::socket(int port) {
			struct sockaddr_in addr;
			memset(&addr, 0, sizeof(addr));
			addr.sin_family = AF_INET;
			addr.sin_port = htons(port);

			addr.sin_addr.s_addr = htonl(INADDR_ANY);

			SocketId s = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (s < 0)
			{
				return Invalid_Socket;
			}

			if (preInitSock(s) != Serialize::NO_ERROR){
				close(s);
				return Invalid_Socket;
			}
			
			if (postInitSock(s) != Serialize::NO_ERROR){
				close(s);
				return Invalid_Socket;
			}

			if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) != 0)
			{
				close(s);
				return Invalid_Socket;
			}

			int result = listen(s, SOMAXCONN);
			if (result != 0) {
				close(s);
				return Invalid_Socket;
			}
			return s;
		}

		std::pair<SocketId, Serialize::StreamError> SocketAPI::accept(SocketId s, TimeOut timeout) {
			struct timeval tv;
			fd_set readfds;

			if (timeout.isInfinite())
			{
				tv.tv_sec = 0;
				tv.tv_usec = 0;
			}
			else
			{
				std::chrono::milliseconds remainder = std::chrono::duration_cast<std::chrono::milliseconds>(timeout.remainder());
				tv.tv_sec = static_cast<long>(remainder.count()) / 1000;
				tv.tv_usec = static_cast<long>(remainder.count()) % 1000 * 1000;
			}

			FD_ZERO(&readfds);
			FD_SET(s, &readfds);

			int retval = select(s + 1, &readfds, NULL, NULL, &tv);
			if (retval > 0) {
				int socket = accept4(s, NULL, NULL, O_NONBLOCK);
				if (socket >= 0)
					return { socket, Serialize::NO_ERROR };
				else
					return { Invalid_Socket, Serialize::NO_CONNECTION };
			}
			else {
				if (retval == 0)
					return { Invalid_Socket, Serialize::TIMEOUT };
				else
					return { Invalid_Socket, getError() };
			}
		}

		std::pair<SocketId, Serialize::StreamError> SocketAPI::connect(const std::string &url, int portNr) {
			//Fill out the information needed to initialize a socket…
			struct sockaddr_in target; //Socket address information

			target.sin_family = AF_INET; // address family Internet
			target.sin_port = htons(portNr); //Port to connect on

			if (inet_pton(AF_INET, url.c_str(), &target.sin_addr) <= 0) {
				return { Invalid_Socket, getError() };
			}

			SocketId s = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //Create socket
			if (s < 0)
			{
				return { Invalid_Socket, getError() };
			}

			if (preInitSock(s) != Serialize::NO_ERROR)
			{
				Serialize::StreamError error = getError();
				close(s);
				return { Invalid_Socket, error };
			}

			//Try connecting...

			if (::connect(s, (struct sockaddr *)&target, sizeof(target)) < 0)
			{
				Serialize::StreamError error = getError();
				close(s);
				return { Invalid_Socket, error };
			}
			
			if (postInitSock(s) != Serialize::NO_ERROR)
			{
				Serialize::StreamError error = getError();
				close(s);
				return { Invalid_Socket, error };
			}

			return { s, Serialize::NO_ERROR };
		}



	}
}

#endif
