#pragma once

#include "../timeout.h"

#include "../streams/streamresult.h"

namespace Engine {
namespace Network {
    using SocketId = unsigned long long;
    const constexpr SocketId Invalid_Socket = -1;

    struct INTERFACES_EXPORT SocketAPI {
        static bool init();
        static void finalize();

        static void closeSocket(SocketId id);
        static std::pair<SocketId, StreamResult> socket(int port);
        static std::pair<SocketId, StreamResult> accept(SocketId s, TimeOut timeout = {});
        static std::pair<SocketId, StreamResult> connect(const std::string &url, int portNr);

        static int send(SocketId id, char *buf, size_t len);
        static int recv(SocketId id, char *buf, size_t len);

        static StreamResult getError();
        static int getOSError();
    };
}
}
