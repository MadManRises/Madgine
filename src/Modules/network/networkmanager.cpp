#include "../moduleslib.h"

#include "networkmanager.h"

#include "../serialize/formatter/safebinaryformatter.h"

#include "../threading/workgroup.h"

namespace Engine {
namespace Network {
    static int sManagerCount = 0;

    NetworkManager::NetworkManager(const std::string &name)
        : SyncManager(name)
        , mSocket(Invalid_Socket)
        , mIsServer(false)
        , mConnectionEstablished(this)
    {
        if (sManagerCount == 0) {
            if (!SocketAPI::init())
                std::terminate();
        }
        ++sManagerCount;
    }

    NetworkManager::NetworkManager(NetworkManager &&other) noexcept
        : SyncManager(std::forward<NetworkManager>(other))
        , mSocket(std::exchange(other.mSocket, Invalid_Socket))
        , mIsServer(std::exchange(other.mIsServer, false))
        , mConnectionEstablished(this)
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

        StreamResult error;

        std::tie(mSocket, error) = SocketAPI::socket(port);

        if (!isConnected()) {
            mConnectionResult.emit(error);
            return error;
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

        StreamResult error;

        std::tie(mSocket, error) = SocketAPI::connect(url, portNr);

        if (!isConnected()) {
            mConnectionResult.emit(error);
            return error;
        }

        mConnectionEstablished.queue(timeout);

        return NetworkManagerResult::SUCCESS;
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
            StreamResult error;
            SocketId sock;
            std::tie(sock, error) = SocketAPI::accept(mSocket);
            while (error != StreamResult::TIMEOUT && (limit == -1 || count < limit)) {
                if (sock != Invalid_Socket) {
                    if (addMasterStream(Serialize::BufferedInOutStream { std::make_unique<NetworkBuffer>(sock, std::make_unique<Serialize::SafeBinaryFormatter>(), *this, createStreamId()) })) {
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
        NetworkManagerResult result;
        if (!isConnected() || !mIsServer)
            result = NetworkManagerResult::NO_SERVER;
        else {
            SocketId sock;
            std::tie(sock, result) = SocketAPI::accept(mSocket, timeout);
            if (sock != Invalid_Socket) {
                Serialize::BufferedInOutStream stream { std::make_unique<NetworkBuffer>(sock, std::make_unique<Serialize::SafeBinaryFormatter>(), *this, createStreamId()) };
                if (!addMasterStream(std::move(stream)))
                    result = stream.state();
            }
        }
        return result;
    }

    bool NetworkManager::isConnected() const
    {
        return mSocket != Invalid_Socket;
    }

    bool NetworkManager::moveMasterStream(
        Serialize::ParticipantId streamId, NetworkManager *target)
    {
        return SyncManager::moveMasterStream(streamId, target);
    }

    Threading::SignalStub<NetworkManagerResult> &NetworkManager::connectionResult()
    {
        return mConnectionResult;
    }

    void NetworkManager::onConnectionEstablished(TimeOut timeout)
    {
        StreamResult error = setSlaveStream(Serialize::BufferedInOutStream { std::make_unique<NetworkBuffer>(mSocket, std::make_unique<Serialize::SafeBinaryFormatter>(), *this) }, true, timeout);

        mConnectionResult.emit(error);
    }
}
}
