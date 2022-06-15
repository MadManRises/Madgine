#pragma once

#include "Madgine/server/serverbase.h"

#include "serialize/network/networkmanager.h"

namespace MMOLobby {	

		struct MMOLOBBY_EXPORT Server : Engine::Server::ServerBase {		
			Server();
			~Server();

			void moveConnection(Engine::Serialize::ParticipantId id, Engine::Network::NetworkManager *to);

			Engine::Network::NetworkManager& network();

		protected:			
			void checkConnections();

			virtual void performCommand(const std::string &cmd) override;			

		private:
			Engine::Network::NetworkManager mNetwork;
		};

}

REGISTER_TYPE(MMOLobby::Server)