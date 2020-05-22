#include "../interfaceslib.h"

#if WINDOWS

#    include "socketapi.h"

#    include <WS2tcpip.h>
#    include <WinSock2.h>

#    undef NO_ERROR

namespace Engine {
namespace Network {
    bool SocketAPI::init()
    {
        WSADATA w;

        int error = WSAStartup(MAKEWORD(2, 2), &w);

        if (error) {
            return false;
        }

        if (w.wVersion != MAKEWORD(2, 2)) {
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

    int SocketAPI::send(SocketId id, char *buf, size_t len)
    {
        return ::send(id, buf, static_cast<int>(len), 0);
    }

    int SocketAPI::recv(SocketId id, char *buf, size_t len)
    {
        return ::recv(id, buf, static_cast<int>(len), 0);
    }

    SocketAPIResult SocketAPI::getError()
    {
        int error = WSAGetLastError();
        switch (error) {
        case WSAECONNREFUSED:
            return SocketAPIResult::CONNECTION_REFUSED;
        case WSAEWOULDBLOCK:
            return SocketAPIResult::WOULD_BLOCK;
        default:
            return SocketAPIResult::UNKNOWN_ERROR;
        }
    }

    int SocketAPI::getOSError()
    {
        return WSAGetLastError();
    }

    std::pair<SocketId, SocketAPIResult> SocketAPI::socket(int port)
    {
        SOCKADDR_IN addr;
        memset(&addr, 0, sizeof addr);
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);

        addr.sin_addr.s_addr = htonl(INADDR_ANY);

        SocketId s = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        if (s == INVALID_SOCKET) {
            return { Invalid_Socket, SocketAPIResult::UNKNOWN_ERROR };
        }

        if (bind(s, LPSOCKADDR(&addr), sizeof addr) == SOCKET_ERROR) {
            closesocket(s);
            return { Invalid_Socket, SocketAPIResult::UNKNOWN_ERROR };
        }

        int result = listen(s, SOMAXCONN);
        if (result == SOCKET_ERROR) {
            closesocket(s);
            return { Invalid_Socket, SocketAPIResult::UNKNOWN_ERROR };
        }
        return { s, SocketAPIResult::SUCCESS };
    }

    std::pair<SocketId, SocketAPIResult> SocketAPI::accept(SocketId s, TimeOut timeout)
    {
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(s, &readSet);
        timeval timeout_s;
        if (timeout.isInfinite()) {
            timeout_s.tv_sec = 0;
            timeout_s.tv_usec = 0;
        } else {
            std::chrono::milliseconds remainder = std::chrono::duration_cast<std::chrono::milliseconds>(timeout.remainder());
            timeout_s.tv_sec = static_cast<long>(remainder.count()) / 1000;
            timeout_s.tv_usec = static_cast<long>(remainder.count()) % 1000 * 1000;
        }
        if (select(static_cast<int>(s), &readSet, nullptr, nullptr, &timeout_s) > 0) {
            SocketId sock = ::accept(s, nullptr, nullptr);
            u_long iMode = 1;
            if (ioctlsocket(sock, FIONBIO, &iMode)) {
                closesocket(sock);
                return { Invalid_Socket, SocketAPIResult::UNKNOWN_ERROR };
            }
            return { sock, SocketAPIResult::SUCCESS };
        }
        return { Invalid_Socket, SocketAPIResult::TIMEOUT };
    }

    std::pair<SocketId, SocketAPIResult> SocketAPI::connect(const std::string &url, int portNr)
    {
        //Fill out the information needed to initialize a socket…
        SOCKADDR_IN target; //Socket address information

        target.sin_family = AF_INET; // address family Internet
        target.sin_port = htons(portNr); //Port to connect on
        InetPton(AF_INET, url.c_str(), &target.sin_addr.s_addr);

        SocketId s = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //Create socket

        if (s == INVALID_SOCKET) {
            return { Invalid_Socket, getError() };
        }

        //Try connecting...

        if (::connect(s, reinterpret_cast<SOCKADDR *>(&target), sizeof target) == SOCKET_ERROR) {
            SocketAPIResult error = getError();
            closesocket(s);
            return { Invalid_Socket, error };
        }

        u_long iMode = 1;
        if (ioctlsocket(s, FIONBIO, &iMode)) {
            SocketAPIResult error = getError();
            closesocket(s);
            return { Invalid_Socket, error };
        }

        return { s, SocketAPIResult::SUCCESS };
    }
}
}

#endif
