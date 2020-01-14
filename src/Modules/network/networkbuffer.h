#pragma once

#include "../serialize/streams/buffered_streambuf.h"

#include "Interfaces/socket/socketapi.h"

namespace Engine {
namespace Network {
    struct MODULES_EXPORT NetworkBuffer : Serialize::buffered_streambuf {
        NetworkBuffer(SocketId socket, std::unique_ptr<Serialize::Formatter> format, Serialize::SyncManager &mgr, Serialize::ParticipantId id = 0);
        NetworkBuffer(const NetworkBuffer &) = delete;
        NetworkBuffer(NetworkBuffer &&other) noexcept;
        virtual ~NetworkBuffer();

    protected:
        // Geerbt über buffered_streambuf
        StreamError getError() override;

        int recv(char *, size_t) override;

        int send(char *, size_t) override;

    private:
        SocketId mSocket; // = SOCKET
    };
}
}
