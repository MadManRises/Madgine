
#include "mmolobbylib.h"

#include "server.h"

#include "Meta/serialize/formatter/xmlformatter.h"

namespace MMOLobby {

Server::Server()
    : ServerBase("MMOLobby-Server")
    , mNetwork("MMOLobby-Server")
{
    LOG("MMOLobby Main-Server started!");

    if (Engine::Network::NetworkManagerResult result = mNetwork.startServer(12345)) {
        LOG_ERROR(result);
        shutdown();
    }

    mTaskQueue.addRepeatedTask([this]() { checkConnections(); }, std::chrono::milliseconds(400));
}

Server::~Server()
{
    LOG("MMOLobby Main-Server shutdown!");
}

void Server::moveConnection(Engine::Serialize::ParticipantId id, Engine::Network::NetworkManager *to)
{
    mNetwork.moveMasterStream(id, to);
}

Engine::Network::NetworkManager &Server::network()
{
    return mNetwork;
}

void Server::checkConnections()
{
    mNetwork.acceptConnections([]() -> std::unique_ptr<Engine::Serialize::Formatter> { return std::make_unique<Engine::Serialize::XMLFormatter>(); });
    if (Engine::Serialize::StreamResult result = mNetwork.sendAndReceiveMessages(); result.mState != Engine::Serialize::StreamState::OK) {
        LOG_ERROR(result);
    }
}

void Server::performCommand(const std::string &cmd)
{
    {
        ServerBase::performCommand(cmd);
    }
}

}
