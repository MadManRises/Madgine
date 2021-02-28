#include "networklib.h"

#include "networkmanager.h"

#include "Meta/serialize/formatter/safebinaryformatter.h"


namespace Engine {
namespace Network {
    static int sManagerCount = 0;

    NetworkManager::NetworkManager(const std::string &name)
        : SyncManager(name)
        , mSocket(Invalid_Socket)
        , mServerSocket(Invalid_Socket)
    {
        if (sManagerCount == 0) {
            if (SocketAPI::init() != SocketAPIResult::SUCCESS)
                std::terminate();
        }
        ++sManagerCount;
    }

    NetworkManager::NetworkManager(NetworkManager &&other) noexcept
        : SyncManager(std::forward<NetworkManager>(other))
        , mSocket(std::exchange(other.mSocket, Invalid_Socket))
        , mServerSocket(std::exchange(other.mServerSocket, Invalid_Socket))
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

        std::tie(mSocket, error) = SocketAPI::connect(url, portNr);

        if (!isConnected()) {
            NetworkManagerResult result = recordSocketError(error);
            return result;
        }

        NetworkManagerResult result = setSlaveStream(Serialize::BufferedInOutStream { std::make_unique<NetworkBuffer>(mSocket), std::make_unique<Serialize::SafeBinaryFormatter>(), *this }, true, timeout);

        return result;
    }

    void NetworkManager::close()
    {
        if (isConnected() || isServer()) {
            removeAllStreams();
            if (mServerSocket != Invalid_Socket) {
                SocketAPI::closeSocket(mServerSocket);
                mServerSocket = Invalid_Socket;
            }
            mSocket = Invalid_Socket;
        }
    }

    int NetworkManager::acceptConnections(int limit, TimeOut timeout)
    {
        int count = 0;
        if (isServer()) {
            SocketAPIResult error;
            SocketId sock;
            std::tie(sock, error) = SocketAPI::accept(mServerSocket, timeout);
            while (error != SocketAPIResult::TIMEOUT && (limit == -1 || count < limit)) {
                if (sock != Invalid_Socket) {
                    if (addMasterStream(Serialize::BufferedInOutStream { std::make_unique<NetworkBuffer>(sock), std::make_unique<Serialize::SafeBinaryFormatter>(), *this, createStreamId() }) == Serialize::SyncManagerResult::SUCCESS) {
                        ++count;
                    }
                }
                std::tie(sock, error) = SocketAPI::accept(mSocket);
            }
        }
        return count;
    }

    NetworkManagerResult NetworkManager::acceptConnection(TimeOut timeout)
    {
        if (!isServer())
            return NetworkManagerResult::NO_SERVER;

        SocketAPIResult error;
        SocketId sock;
        std::tie(sock, error) = SocketAPI::accept(mServerSocket, timeout);
        if (sock == Invalid_Socket)
            return recordSocketError(error);

        Serialize::BufferedInOutStream stream { std::make_unique<NetworkBuffer>(sock), std::make_unique<Serialize::SafeBinaryFormatter>(), *this, createStreamId() };
        return addMasterStream(std::move(stream));
    }

    bool NetworkManager::isConnected() const
    {
        return mSocket != Invalid_Socket;
    }

    bool NetworkManager::isServer() const
    {
        return mServerSocket != Invalid_Socket;
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
