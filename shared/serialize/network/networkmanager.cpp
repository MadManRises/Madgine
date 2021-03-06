#include "networklib.h"

#include "networkmanager.h"

#include "Meta/serialize/formatter/safebinaryformatter.h"


namespace Engine {
namespace Network {
    static int sManagerCount = 0;

    NetworkManager::NetworkManager(const std::string &name)
        : SyncManager(name)        
    {
        if (sManagerCount == 0) {
            if (SocketAPI::init() != SocketAPIResult::SUCCESS)
                std::terminate();
        }
        ++sManagerCount;
    }

    NetworkManager::NetworkManager(NetworkManager &&other) noexcept
        : SyncManager(std::forward<NetworkManager>(other))        
        , mServerSocket(std::move(other.mServerSocket))
    {
        ++sManagerCount;
    }

    NetworkManager::~NetworkManager()
    {
        close();
        --sManagerCount;
        if (sManagerCount == 0) {
            SocketAPI::finalize();
        }
    }

    NetworkManagerResult NetworkManager::startServer(int port)
    {
        if (isServer())
            return NetworkManagerResult::ALREADY_CONNECTED;

        SocketAPIResult error;

        std::tie(mServerSocket, error) = SocketAPI::socket(port);

        if (!isServer()) {
            NetworkManagerResult result = recordSocketError(error);
            return result;
        }

        return NetworkManagerResult::SUCCESS;
    }

    NetworkManagerResult NetworkManager::connect(const std::string &url, int portNr, TimeOut timeout)
    {
        if (isConnected()) {
            return NetworkManagerResult::ALREADY_CONNECTED;
        }

        SocketAPIResult error;

        Socket socket;
        std::tie(socket, error) = SocketAPI::connect(url, portNr);

        if (!socket) {
            NetworkManagerResult result = recordSocketError(error);
            return result;
        }

        NetworkManagerResult result = setSlaveStream(Serialize::BufferedInOutStream { std::make_unique<NetworkBuffer>(std::move(socket)), std::make_unique<Serialize::SafeBinaryFormatter>(), *this }, true, timeout);

        return result;
    }

    void NetworkManager::close()
    {
        if (isConnected() || isServer()) {
            removeAllStreams();
            if (mServerSocket) {
                mServerSocket.close();
            }
        }
    }

    int NetworkManager::acceptConnections(int limit, TimeOut timeout)
    {
        int count = 0;
        if (isServer()) {
            SocketAPIResult error;
            Socket sock;
            std::tie(sock, error) = mServerSocket.accept(timeout);
            while (error != SocketAPIResult::TIMEOUT && (limit == -1 || count < limit)) {
                if (sock) {
                    if (addMasterStream(Serialize::BufferedInOutStream { std::make_unique<NetworkBuffer>(std::move(sock)), std::make_unique<Serialize::SafeBinaryFormatter>(), *this, createStreamId() }) == Serialize::SyncManagerResult::SUCCESS) {
                        ++count;
                    }
                }
                std::tie(sock, error) = mServerSocket.accept(timeout);
            }
        }
        return count;
    }

    NetworkManagerResult NetworkManager::acceptConnection(TimeOut timeout)
    {
        if (!isServer())
            return NetworkManagerResult::NO_SERVER;

        SocketAPIResult error;
        Socket sock;
        std::tie(sock, error) = mServerSocket.accept(timeout);
        if (!sock)
            return recordSocketError(error);

        Serialize::BufferedInOutStream stream { std::make_unique<NetworkBuffer>(std::move(sock)), std::make_unique<Serialize::SafeBinaryFormatter>(), *this, createStreamId() };
        return addMasterStream(std::move(stream));
    }

    bool NetworkManager::isConnected() const
    {
        return !isMaster();
    }

    bool NetworkManager::isServer() const
    {
        return bool(mServerSocket);
    }

    NetworkManagerResult NetworkManager::moveMasterStream(
        Serialize::ParticipantId streamId, NetworkManager *target)
    {
        return SyncManager::moveMasterStream(streamId, target);
    }

    SocketAPIResult NetworkManager::getSocketAPIError() const
    {
        return mSocketAPIError;
    }

    NetworkManagerResult NetworkManager::recordSocketError(SocketAPIResult error)
    {
        mSocketAPIError = error;
        return NetworkManagerResult::SOCKET_ERROR;
    }

}
}
