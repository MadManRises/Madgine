#include "interfaceslib.h"

#ifdef __linux__

#include "socketapi.h"

#include <sys/socket.h>
#include <unistd.h>
#include <netinet/ip.h>
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

		size_t SocketAPI::send(SocketId id, char *buf, size_t len)
		{
			return ::send(id, buf, len, MSG_NOSIGNAL);
		}

		size_t SocketAPI::recv(SocketId id, char *buf, size_t len)
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

			int flags = fcntl(s, F_GETFL, 0);
			if (flags < 0) {
				close(s);
				return Invalid_Socket;
			}
			flags |= O_NONBLOCK;
			if (fcntl(s, F_SETFL, flags) != 0) {
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

		std::pair<SocketId, Serialize::StreamError> SocketAPI::accept(SocketId s, int timeout) {
			struct timeval tv;
			fd_set readfds;

			tv.tv_sec = timeout / 1000;
			tv.tv_usec = (timeout % 1000) * 1000;

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

			

			//Try connecting...

			if (::connect(s, (struct sockaddr *)&target, sizeof(target)) < 0)
			{
				Serialize::StreamError error = getError();
				close(s);
				return { Invalid_Socket, error };
			}

			int flags = fcntl(s, F_GETFL, 0);
			if (flags < 0) {
				Serialize::StreamError error = getError();
				close(s);
				return { Invalid_Socket, error };
			}
			flags |= O_NONBLOCK;
			if (fcntl(s, F_SETFL, flags) != 0) {
				Serialize::StreamError error = getError();
				close(s);
				return { Invalid_Socket, error };
			}


			return { s, Serialize::NO_ERROR };
		}

	}
}

#endif