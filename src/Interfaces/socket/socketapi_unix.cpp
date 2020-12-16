#include "../interfaceslib.h"

#if UNIX

#    include "socketapi.h"

#    include <arpa/inet.h>
#    include <fcntl.h>
#    include <netinet/ip.h>
#    include <netinet/tcp.h>
#    include <sys/socket.h>
#    include <unistd.h>

namespace Engine {

SocketAPIResult SocketAPI::init()
{
    return SocketAPIResult::SUCCESS;
}

void SocketAPI::finalize()
{
}

void SocketAPI::closeSocket(SocketId id)
{
    int result = close(id);
    assert(result == 0);
}

int SocketAPI::send(SocketId id, char *buf, size_t len)
{
    return ::send(id, buf, len, 0);
}

int SocketAPI::recv(SocketId id, char *buf, size_t len)
{
    return ::read(id, buf, len);
}

SocketAPIResult SocketAPI::getError(const char *op)
{
    int error = errno;
    switch (error) {
    case EWOULDBLOCK:
        return SocketAPIResult::WOULD_BLOCK;
    case EADDRINUSE:
        return SocketAPIResult::ALREADY_IN_USE;
    case EBADF:
    case ENOTSOCK:
    case EOPNOTSUPP:
        throw 0;
    default:
        fprintf(stderr, "Unknown Linux Socket-Error-Code from %s: %d", op, error);
        fflush(stderr);
        return SocketAPIResult::UNKNOWN_ERROR;
    }
}

int SocketAPI::getOSError()
{
    return errno;
}

SocketAPIResult preInitSock(SocketId s)
{
#    if !EMSCRIPTEN
    int on = 1;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        return SocketAPI::getError("setsockopt");
    }
    #    if LINUX || ANDROID
        if (setsockopt(s, SOL_TCP, TCP_NODELAY, &on, sizeof(on)) < 0) {
            return SocketAPI::getError("setsockopt");
        }
    #endif
#    endif

    return SocketAPIResult::SUCCESS;
}

SocketAPIResult postInitSock(SocketId s)
{
    int flags = fcntl(s, F_GETFL, 0);
    if (flags < 0) {
        return SocketAPI::getError("fcntl");
    }
    flags |= O_NONBLOCK;
    if (fcntl(s, F_SETFL, flags) != 0) {
        return SocketAPI::getError("fcntl");
    }
    return SocketAPIResult::SUCCESS;
}

std::pair<SocketId, SocketAPIResult> SocketAPI::socket(int port)
{
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    SocketId s = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s < 0) {
        return { Invalid_Socket, getError("socket") };
    }

    if (SocketAPIResult result = preInitSock(s); result != SocketAPIResult::SUCCESS) {
        closeSocket(s);
        return { Invalid_Socket, result };
    }

    if (SocketAPIResult result = postInitSock(s); result != SocketAPIResult::SUCCESS) {
        closeSocket(s);
        return { Invalid_Socket, result };
    }

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        SocketAPIResult result = getError("bind");
        closeSocket(s);
        return { Invalid_Socket, result };
    }

    int result = listen(s, SOMAXCONN);
    if (result != 0) {
        SocketAPIResult result = getError("listen");
        closeSocket(s);
        return { Invalid_Socket, result };
    }
    return { s, SocketAPIResult::SUCCESS };
}

std::pair<SocketId, SocketAPIResult> SocketAPI::accept(SocketId s, TimeOut timeout)
{
    struct timeval tv;
    fd_set readfds;

    if (timeout.isInfinite()) {
        tv.tv_sec = 0;
        tv.tv_usec = 0;
    } else {
        std::chrono::milliseconds remainder = std::chrono::duration_cast<std::chrono::milliseconds>(timeout.remainder());
        tv.tv_sec = static_cast<long>(remainder.count()) / 1000;
        tv.tv_usec = static_cast<long>(remainder.count()) % 1000 * 1000;
    }

    FD_ZERO(&readfds);
    FD_SET(s, &readfds);

    int retval = select(s + 1, &readfds, NULL, NULL, &tv);
    if (retval > 0) {
#if OSX || IOS
        int socket = ::accept(s, NULL, NULL);
#else
        int socket = accept4(s, NULL, NULL, O_NONBLOCK);
#endif
        if (socket >= 0)
            return { socket, SocketAPIResult::SUCCESS };
        else
            return { Invalid_Socket, getError("accept") };
    } else {
        if (retval == 0)
            return { Invalid_Socket, SocketAPIResult::TIMEOUT };
        else
            return { Invalid_Socket, getError("select") };
    }
}

std::pair<SocketId, SocketAPIResult> SocketAPI::connect(const std::string &url, int portNr)
{
    //Fill out the information needed to initialize a socket…
    struct sockaddr_in target; //Socket address information

    target.sin_family = AF_INET; // address family Internet
    target.sin_port = htons(portNr); //Port to connect on

    if (inet_pton(AF_INET, url.c_str(), &target.sin_addr) <= 0) {
        return { Invalid_Socket, getError("inet_pton") };
    }

    SocketId s = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //Create socket
    if (s < 0) {
        return { Invalid_Socket, getError("socket") };
    }

    if (SocketAPIResult result = preInitSock(s); result != SocketAPIResult::SUCCESS) {
        closeSocket(s);
        return { Invalid_Socket, result };
    }

    //Try connecting...

    if (::connect(s, (struct sockaddr *)&target, sizeof(target)) < 0) {
        SocketAPIResult error = getError("connect");
        closeSocket(s);
        return { Invalid_Socket, error };
    }

    if (SocketAPIResult result = postInitSock(s); result != SocketAPIResult::SUCCESS) {
        closeSocket(s);
        return { Invalid_Socket, result };
    }

    return { s, SocketAPIResult::SUCCESS };
}

}

#endif
