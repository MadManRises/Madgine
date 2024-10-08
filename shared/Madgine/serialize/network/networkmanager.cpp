#include "networklib.h"

#include "networkmanager.h"

#include "networkbuffer.h"

#include "Meta/serialize/streams/formattedbufferedstream.h"

#include "Meta/serialize/streams/syncstreamdata.h"

#include "Meta/serialize/streams/buffered_streambuf.h"

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

        SocketAPIResult error = mServerSocket.open(port);

        if (!isServer()) {
            NetworkManagerResult result = recordSocketError(error);
            return result;
        }

        return NetworkManagerResult::SUCCESS;
    }

    NetworkManagerResult NetworkManager::connect(std::string_view url, int portNr, std::unique_ptr<Serialize::Formatter> (*format)(), TimeOut timeout)
    {
        if (isConnected()) {
            return NetworkManagerResult::ALREADY_CONNECTED;
        }

        Socket socket;
        SocketAPIResult error = socket.connect(url, portNr);

        if (!socket) {
            NetworkManagerResult result = recordSocketError(error);
            return result;
        }

        NetworkManagerResult result = setSlaveStream(Serialize::FormattedBufferedStream { format(), std::make_unique<Engine::Serialize::buffered_streambuf>(std::make_unique<NetworkBuffer>(std::move(socket))), createStreamData() }, true, timeout);

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

    int NetworkManager::acceptConnections(std::unique_ptr<Serialize::Formatter> (*format)(), int limit, TimeOut timeout)
    {
        int count = 0;
        if (isServer()) {
            SocketAPIResult error;
            Socket sock;
            std::tie(sock, error) = mServerSocket.accept(timeout);
            while (error != SocketAPIResult::TIMEOUT && (limit == -1 || count < limit)) {
                if (sock) {
                    if (addMasterStream(Serialize::FormattedBufferedStream { format(), std::make_unique<Engine::Serialize::buffered_streambuf>(std::make_unique<NetworkBuffer>(std::move(sock))), createStreamData() }) == Serialize::SyncManagerResult::SUCCESS) {
                        ++count;
                    }
                }
                std::tie(sock, error) = mServerSocket.accept(timeout);
            }
        }
        return count;
    }

    NetworkManagerResult NetworkManager::acceptConnection(std::unique_ptr<Serialize::Formatter> (*format)(), TimeOut timeout)
    {
        if (!isServer())
            return NetworkManagerResult::NO_SERVER;

        SocketAPIResult error;
        Socket sock;
        std::tie(sock, error) = mServerSocket.accept(timeout);
        if (!sock)
            return recordSocketError(error);

        Serialize::FormattedBufferedStream stream { format(), std::make_unique<Engine::Serialize::buffered_streambuf>(std::make_unique<NetworkBuffer>(std::move(sock))), createStreamData() };
        return addMasterStream(std::move(stream));
    }

    bool NetworkManager::isConnected() const
    {
        return !isMaster() && !mSlaveStreamInvalid;
    }

    bool NetworkManager::isServer() const
    {
        return static_cast<bool>(mServerSocket);
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
