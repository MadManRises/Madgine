#include "networklib.h"

#include "networkmanager.h"

#include "Meta/serialize/formatter/safebinaryformatter.h"

#include "Modules/threading/workgroup.h"

namespace Engine {
namespace Network {
    static int sManagerCount = 0;

    NetworkManager::NetworkManager(const std::string &name)
        : SyncManager(name)
        , mSocket(Invalid_Socket)
        , mIsServer(false)
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
        , mIsServer(std::exchange(other.mIsServer, false))
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
        if (isConnected())
            return NetworkManagerResult::ALREADY_CONNECTED;

        SocketAPIResult error;

        std::tie(mSocket, error) = SocketAPI::socket(port);

        if (!isConnected()) {
            NetworkManagerResult result = recordSocketError(error);
            mConnectionResult.emit(result);
            return result;
        }

        mIsServer = true;
        return NetworkManagerResult::SUCCESS;
    }

    NetworkManagerResult NetworkManager::connect(const std::string &url, int portNr, TimeOut timeout)
    {
        if (isConnected()) {
            mConnectionResult.emit(NetworkManagerResult::ALREADY_CONNECTED);
            return NetworkManagerResult::ALREADY_CONNECTED;
        }

        SocketAPIResult error;

        std::tie(mSocket, error) = SocketAPI::connect(url, portNr);

        if (!isConnected()) {
            NetworkManagerResult result = recordSocketError(error);
            mConnectionResult.emit(result);
            return result;
        }

        NetworkManagerResult result = setSlaveStream(Serialize::BufferedInOutStream { std::make_unique<NetworkBuffer>(mSocket, std::make_unique<Serialize::SafeBinaryFormatter>(), *this) }, true, timeout);

        mConnectionResult.emit(result);

        return result;
    }

    void NetworkManager::close()
    {
        if (isConnected()) {
            removeAllStreams();
            if (mIsServer) {
                SocketAPI::closeSocket(mSocket);
                mIsServer = false;
            }
            mSocket = Invalid_Socket;
        }
    }

    int NetworkManager::acceptConnections(int limit)
    {
        int count = 0;
        if (isConnected() && mIsServer) {
            SocketAPIResult error;
            SocketId sock;
            std::tie(sock, error) = SocketAPI::accept(mSocket);
            while (error != SocketAPIResult::TIMEOUT && (limit == -1 || count < limit)) {
                if (sock != Invalid_Socket) {
                    if (addMasterStream(Serialize::BufferedInOutStream { std::make_unique<NetworkBuffer>(sock, std::make_unique<Serialize::SafeBinaryFormatter>(), *this, createStreamId()) }) == Serialize::SyncManagerResult::SUCCESS) {
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
        if (!isConnected() || !mIsServer)
            return NetworkManagerResult::NO_SERVER;

        SocketAPIResult error;
        SocketId sock;
        std::tie(sock, error) = SocketAPI::accept(mSocket, timeout);
        if (sock == Invalid_Socket)
            return recordSocketError(error);

        Serialize::BufferedInOutStream stream { std::make_unique<NetworkBuffer>(sock, std::make_unique<Serialize::SafeBinaryFormatter>(), *this, createStreamId()) };
        return addMasterStream(std::move(stream));        
    }

    bool NetworkManager::isConnected() const
    {
        return mSocket != Invalid_Socket;
    }

    NetworkManagerResult NetworkManager::moveMasterStream(
        Serialize::ParticipantId streamId, NetworkManager *target)
    {
        return SyncManager::moveMasterStream(streamId, target);
    }

    Threading::SignalStub<NetworkManagerResult> &NetworkManager::connectionResult()
    {
        return mConnectionResult;
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
