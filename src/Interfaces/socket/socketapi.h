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

struct SocketId {

    SocketId(unsigned long long socket = Invalid_Socket)
        : mSocket(socket)
    {
    }

    SocketId(const SocketId &) = delete;
    SocketId(SocketId &&other)
        : mSocket(std::exchange(other.mSocket, Invalid_Socket))
    {
    }

    ~SocketId()
    {
        assert(!(*this));
    }

    SocketId& operator=(SocketId&& other) {
        assert(!(*this));
        mSocket = std::exchange(other.mSocket, Invalid_Socket);
        return *this;
    }

    explicit operator bool() const
    {
        return mSocket != Invalid_Socket;
    }

private:
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

    static void closeSocket(SocketId &id);
    static std::pair<SocketId, SocketAPIResult> socket(int port);
    static std::pair<SocketId, SocketAPIResult> accept(const SocketId &s, TimeOut timeout = {});
    static std::pair<SocketId, SocketAPIResult> connect(const std::string &url, int portNr);

    static int send(const SocketId &id, const char *buf, size_t len);
    static int recv(const SocketId &id, char *buf, size_t len);

    static int in_available(const SocketId &id);

    static SocketAPIResult getError(const char *operation);
    static int getOSError();
};

}
