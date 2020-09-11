#pragma once

#include "../util/timeout.h"

#include "../util/genericresult.h"

namespace Engine {

    ENUM_BASE(SocketAPIResult, GenericResult, 
        WOULD_BLOCK,
        TIMEOUT,
        CONNECTION_REFUSED,
        ALREADY_IN_USE,
        API_VERSION_MISMATCH
    );


    using SocketId = unsigned long long;
    const constexpr SocketId Invalid_Socket = -1;

    struct INTERFACES_EXPORT SocketAPI {
        static SocketAPIResult init();
        static void finalize();

        static void closeSocket(SocketId id);
        static std::pair<SocketId, SocketAPIResult> socket(int port);
        static std::pair<SocketId, SocketAPIResult> accept(SocketId s, TimeOut timeout = {});
        static std::pair<SocketId, SocketAPIResult> connect(const std::string &url, int portNr);

        static int send(SocketId id, char *buf, size_t len);
        static int recv(SocketId id, char *buf, size_t len);

        static SocketAPIResult getError(const char *operation);
        static int getOSError();
    };

}
