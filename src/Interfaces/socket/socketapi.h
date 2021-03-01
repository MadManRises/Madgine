#pragma once

#include "Generic/timeout.h"

#include "Generic/genericresult.h"

namespace Engine {

ENUM_BASE(SocketAPIResult, GenericResult,
    WOULD_BLOCK,
    TIMEOUT,
    CONNECTION_REFUSED,
    ALREADY_IN_USE,
    API_VERSION_MISMATCH);

struct INTERFACES_EXPORT Socket {
    Socket() = default;
    Socket(const Socket &) = delete;
    Socket(Socket &&other)
        : mSocket(std::exchange(other.mSocket, Invalid_Socket))
    {
    }

    ~Socket()
    {
        if (*this)
            close();
    }

    void close();

    int send(const char *buf, size_t len) const;
    int recv(char *buf, size_t len) const;

    std::pair<Socket, SocketAPIResult> accept(TimeOut timeout = {}) const;

    int in_available() const;

    Socket& operator=(Socket&& other) {
        assert(!(*this));
        mSocket = std::exchange(other.mSocket, Invalid_Socket);
        return *this;
    }

    explicit operator bool() const
    {
        return mSocket != Invalid_Socket;
    }

private:
    explicit Socket(unsigned long long socket)
        : mSocket(socket)
    {
    }

    operator unsigned long long() const
    {
        return mSocket;
    }


    friend struct SocketAPI;

    static const constexpr unsigned long long Invalid_Socket = -1;
    unsigned long long mSocket = Invalid_Socket;
};

struct INTERFACES_EXPORT SocketAPI {
    static SocketAPIResult init();
    static void finalize();

    static std::pair<Socket, SocketAPIResult> socket(int port);
    static std::pair<Socket, SocketAPIResult> connect(const std::string &url, int portNr);


    static SocketAPIResult getError(const char *operation);
    static int getOSError();
};

}
