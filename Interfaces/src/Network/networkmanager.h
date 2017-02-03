#pragma once

#include "Serialize\serializemanager.h"
#include "networkstream.h"

namespace Engine {
	namespace Network {

		class INTERFACES_EXPORT NetworkManager : 
			public Serialize::SerializeManager{

		public:
			NetworkManager();
			NetworkManager(const NetworkManager &) = delete;
			virtual ~NetworkManager();


			void operator=(const NetworkManager &) = delete;

			bool startServer(int portNr);
			bool connect(const std::string &url, int portNr, int timeout = 0);

			void close();


			bool acceptConnection(int timeout);
			void acceptConnections();

			int clientCount();

		protected:
			virtual void onSlaveStreamRemoved(Serialize::BufferedInOutStream *stream) override;
			virtual void onMasterStreamRemoved(Serialize::BufferedInOutStream *stream) override;

		private:
			UINT_PTR mSocket; // = SOCKET

			bool mIsServer;
			//std::string mURL;
			int mPort;

			std::list<NetworkStream> mStreams;
			NetworkStream *mSlaveStream;

			//static constexpr UINT sMessageSignature = 1048;

			static int sManagerCount;

		};

	}
}

