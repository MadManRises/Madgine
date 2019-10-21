#include "../moduleslib.h"

#include "networkmanager.h"

#include "../serialize/formatter/safebinaryformatter.h"

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

    bool NetworkManager::startServer(int port)
    {
        if (isConnected())
            return false;

        mSocket = SocketAPI::socket(port);

        if (isConnected()) {
            mIsServer = true;
            return true;
        }
        return false;
    }

    StreamError NetworkManager::connect(const std::string &url, int portNr, TimeOut timeout)
    {
        if (isConnected()) {
            mConnectionResult.emit(ALREADY_CONNECTED);
            return ALREADY_CONNECTED;
        }

        StreamError error;

        std::tie(mSocket, error) = SocketAPI::connect(url, portNr);

        if (!isConnected()) {
            mConnectionResult.emit(error);
            return error;
        }

        mConnectionEstablished.queue(timeout);

        return NO_ERROR;
    }

    void NetworkManager::connect_async(const std::string &url, int portNr, TimeOut timeout)
    {
        std::thread(&NetworkManager::connect, this, url, portNr, timeout).detach();
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
        if (isConnected()) {
            if (mIsServer) {
                StreamError error;
                SocketId sock;
                std::tie(sock, error) = SocketAPI::accept(mSocket);
                while (error != TIMEOUT && (limit == -1 || count < limit)) {
                    if (sock != Invalid_Socket) {
                        if (addMasterStream(Serialize::BufferedInOutStream { std::make_unique<NetworkBuffer>(sock, std::make_unique<Serialize::SafeBinaryFormatter>(), *this, createStreamId()) })) {
                            ++count;
                        }
                    }
                    std::tie(sock, error) = SocketAPI::accept(mSocket);
                }
            }
        }
        return count;
    }

    StreamError NetworkManager::acceptConnection(TimeOut timeout)
    {
        if (isConnected()) {
            if (mIsServer) {
                StreamError error;
                SocketId sock;
                std::tie(sock, error) = SocketAPI::accept(mSocket, timeout);
                if (sock != Invalid_Socket) {
                    Serialize::BufferedInOutStream stream { std::make_unique<NetworkBuffer>(sock, std::make_unique<Serialize::SafeBinaryFormatter>(), *this, createStreamId()) };
                    if (addMasterStream(std::move(stream))) {
                        return NO_ERROR;
                    }
                    error = stream.error();
                }
                return error;
            }
        }
        return NO_SERVER;
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

    SignalSlot::SignalStub<StreamError> &NetworkManager::connectionResult()
    {
        return mConnectionResult;
    }

    void NetworkManager::onConnectionEstablished(TimeOut timeout)
    {
        bool success = setSlaveStream(Serialize::BufferedInOutStream { std::make_unique<NetworkBuffer>(mSocket, std::make_unique<Serialize::SafeBinaryFormatter>(), *this) }, true, timeout);

        mConnectionResult.emit(success ? NO_ERROR : UNKNOWN_ERROR);
    }
}
}
