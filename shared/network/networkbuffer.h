#pragma once

#include "Modules/serialize/streams/buffered_streambuf.h"

#include "Interfaces/socket/socketapi.h"

namespace Engine {
namespace Network {
    struct MODULES_EXPORT NetworkBuffer : Serialize::buffered_streambuf {
        NetworkBuffer(SocketId socket, std::unique_ptr<Serialize::Formatter> format, Serialize::SyncManager &mgr, Serialize::ParticipantId id = 0);
        NetworkBuffer(const NetworkBuffer &) = delete;
        NetworkBuffer(NetworkBuffer &&other) noexcept;
        virtual ~NetworkBuffer();

    protected:        
        void handleError() override;

        int recv(char *, uint64_t) override;

        int send(char *, uint64_t) override;

    private:
        SocketId mSocket; // = SOCKET
    };
}
}
