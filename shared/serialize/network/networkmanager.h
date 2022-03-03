#pragma once

#include "Meta/serialize/syncmanager.h"

#include "Interfaces/socket/socketapi.h"

namespace Engine {
namespace Network {

    ENUM_BASE(NetworkManagerResult, Serialize::SyncManagerResult,
        ALREADY_CONNECTED,
        NO_SERVER,
        SOCKET_ERROR);

    struct MADGINE_NETWORK_SERIALIZE_EXPORT NetworkManager : Serialize::SyncManager {
        NetworkManager(const std::string &name);
        NetworkManager(const NetworkManager &) = delete;
        NetworkManager(NetworkManager &&) noexcept;
        virtual ~NetworkManager();

        void operator=(const NetworkManager &) = delete;

        NetworkManagerResult startServer(int port);
        NetworkManagerResult connect(const std::string &url, int portNr, std::unique_ptr<Serialize::Formatter> (*format)(), TimeOut timeout = {});

        void close();

        NetworkManagerResult acceptConnection(std::unique_ptr<Serialize::Formatter> (*format)(), TimeOut timeout = {});
        int acceptConnections(std::unique_ptr<Serialize::Formatter> (*format)(), int limit = -1, TimeOut timeout = 0ms);

        bool isConnected() const;
        bool isServer() const;

        NetworkManagerResult moveMasterStream(Serialize::ParticipantId streamId,
            NetworkManager *target);

        SocketAPIResult getSocketAPIError() const;

    protected:
        NetworkManagerResult recordSocketError(SocketAPIResult error);

    private:
        Socket mServerSocket;                

        SocketAPIResult mSocketAPIError = SocketAPIResult::SUCCESS;
    };
}
}
