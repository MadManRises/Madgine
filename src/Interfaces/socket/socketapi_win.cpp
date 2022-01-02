#include "../interfaceslib.h"

#if WINDOWS

#    include "socketapi.h"

#    include <WS2tcpip.h>
#    include <WinSock2.h>

#    undef NO_ERROR

namespace Engine {

SocketAPIResult toResult(int error, const char *op)
{
    switch (error) {
    case WSAECONNREFUSED:
        return SocketAPIResult::CONNECTION_REFUSED;
    case WSAEWOULDBLOCK:
        return SocketAPIResult::WOULD_BLOCK;
    default:
        fprintf(stderr, "Unknown Windows Socket-Error-Code from %s: %d\n", op, error);
        fflush(stderr);
        return SocketAPIResult::UNKNOWN_ERROR;
    }
}

SocketAPIResult SocketAPI::init()
{
    WSADATA w;

    int error = WSAStartup(MAKEWORD(2, 2), &w);

    if (error) {
        return toResult(error, "WSAStartup");
    }

    if (w.wVersion != MAKEWORD(2, 2)) {
        WSACleanup();
        return SocketAPIResult::API_VERSION_MISMATCH;
    }
    return SocketAPIResult::SUCCESS;
}

void SocketAPI::finalize()
{
    WSACleanup();
}

void Socket::close()
{
    assert(mSocket);
    int result = closesocket(mSocket);
    assert(result == 0);
    mSocket = Invalid_Socket;
}

int Socket::send(const char *buf, size_t len) const
{
    return ::send(mSocket, buf, static_cast<int>(len), 0);
}

int Socket::recv(char *buf, size_t len) const
{
    return ::recv(mSocket, buf, static_cast<int>(len), 0);
}

int Socket::in_available() const
{
    u_long bytes_available;
    if (ioctlsocket(mSocket, FIONREAD, &bytes_available) == SOCKET_ERROR)
        return -1;
    return bytes_available;
}

SocketAPIResult SocketAPI::getError(const char *op)
{
    return toResult(WSAGetLastError(), op);
}

int SocketAPI::getOSError()
{
    return WSAGetLastError();
}

std::pair<Socket, SocketAPIResult> SocketAPI::socket(int port)
{
    SOCKADDR_IN addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    SOCKET s = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (s == INVALID_SOCKET) {
        return { Socket {}, getError("socket") };
    }

    Socket sock { s };

    if (bind(sock, LPSOCKADDR(&addr), sizeof addr) == SOCKET_ERROR) {
        SocketAPIResult result = getError("bind");
        sock.close();
        return { std::move(sock), result };
    }

    int result = listen(sock, SOMAXCONN);
    if (result == SOCKET_ERROR) {
        SocketAPIResult result = getError("listen");
        sock.close();
        return { std::move(sock), result };
    }
    return { std::move(sock), SocketAPIResult::SUCCESS };
}

std::pair<Socket, SocketAPIResult> Socket::accept(TimeOut timeout) const
{
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(mSocket, &readSet);
    timeval timeout_s;
    if (timeout.isInfinite()) {
        timeout_s.tv_sec = 0;
        timeout_s.tv_usec = 0;
    } else {
        std::chrono::milliseconds remainder = std::chrono::duration_cast<std::chrono::milliseconds>(timeout.remainder());
        timeout_s.tv_sec = static_cast<long>(remainder.count()) / 1000;
        timeout_s.tv_usec = static_cast<long>(remainder.count()) % 1000 * 1000;
    }
    if (int error = select(static_cast<int>(mSocket), &readSet, nullptr, nullptr, &timeout_s); error <= 0) {
        if (error == 0)
            return { Socket {}, SocketAPIResult::TIMEOUT };
        else
            return { Socket {}, SocketAPI::getError("select") };
    }

    SOCKET s = ::accept(mSocket, nullptr, nullptr);
    assert(s != INVALID_SOCKET);
    Socket sock { s };

    u_long iMode = 1;
    if (ioctlsocket(sock, FIONBIO, &iMode) == SOCKET_ERROR) {
        SocketAPIResult result = SocketAPI::getError("accept");
        sock.close();
        return { std::move(sock), result };
    }
    return { std::move(sock), SocketAPIResult::SUCCESS };
}

std::pair<Socket, SocketAPIResult> SocketAPI::connect(const std::string &url, int portNr)
{
    //Fill out the information needed to initialize a socket…
    SOCKADDR_IN target; //Socket address information

    target.sin_family = AF_INET; // address family Internet
    target.sin_port = htons(portNr); //Port to connect on
    InetPton(AF_INET, url.c_str(), &target.sin_addr.s_addr);

    SOCKET s = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //Create socket

    if (s == INVALID_SOCKET) {
        return { Socket {}, getError("socket") };
    }

    Socket sock { s };

    //Try connecting...

    if (::connect(sock, reinterpret_cast<SOCKADDR *>(&target), sizeof target) == SOCKET_ERROR) {
        SocketAPIResult error = getError("connect");
        sock.close();
        return { std::move(sock), error };
    }

    u_long iMode = 1;
    if (ioctlsocket(sock, FIONBIO, &iMode)) {
        SocketAPIResult error = getError("ioctlsocket");
        sock.close();
        return { std::move(sock), error };
    }

    return { std::move(sock), SocketAPIResult::SUCCESS };
}
}

#endif
