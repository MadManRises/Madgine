#pragma once

#include "Util\MadgineObject.h"
#include "Serialize\serializemanager.h"
#include "networkstream.h"

namespace Engine {
	namespace Network {

		class MADGINE_EXPORT NetworkManager : 
			public Ogre::GeneralAllocatedObject,
			public Serialize::SerializeManager{

		public:
			NetworkManager();
			NetworkManager(const NetworkManager &) = delete;
			virtual ~NetworkManager();


			void operator=(const NetworkManager &) = delete;

			bool startServer(int portNr);
			bool connect(const std::string &url, int portNr, int timeout = 0);

			void close();

			void receiveMessages();
			void acceptConnections();

			int clientCount();

			virtual std::list<Serialize::SerializeOutStream*> getMessageTargets(Serialize::SerializableUnit *unit) override;

		private:
			UINT_PTR mSocket; // = SOCKET

			bool mIsServer;
			//std::string mURL;
			int mPort;

			std::list<NetworkStream> mStreams;

			//static constexpr UINT sMessageSignature = 1048;

			static int sManagerCount;

		};

	}
}

