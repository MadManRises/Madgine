#pragma once

#include "../serialize/serializemanager.h"
#include "../threading/slot.h"
#include "networkbuffer.h"

namespace Engine
{
	namespace Network
	{
		class MODULES_EXPORT NetworkManager : public Serialize::SerializeManager
		{
		public:
			NetworkManager(const std::string& name);
			NetworkManager(const NetworkManager&) = delete;
			NetworkManager(NetworkManager&&) noexcept;
			virtual ~NetworkManager();


			void operator=(const NetworkManager&) = delete;

			bool startServer(int port);
			StreamError connect(const std::string& url, int portNr, TimeOut timeout = {});
			void connect_async(const std::string& url, int portNr, TimeOut timeout = {});

			void close();


			StreamError acceptConnection(TimeOut timeout);
			int acceptConnections(int limit = -1);



			bool isConnected() const;

			 bool moveMasterStream(Serialize::ParticipantId streamId,
                                              NetworkManager *target);

			SignalSlot::SignalStub<StreamError> &connectionResult();

		protected:

			void onConnectionEstablished(TimeOut timeout);

		private:
			SocketId mSocket;

			bool mIsServer;

			//std::map<Serialize::ParticipantId, NetworkStream> mStreams;
			//std::unique_ptr<NetworkStream> mSlaveStream;

			//static constexpr UINT sMessageSignature = 1048;

			static int sManagerCount;

			SignalSlot::Signal<StreamError> mConnectionResult;
                        Threading::Slot<&NetworkManager::onConnectionEstablished>
			mConnectionEstablished;
		};
	}
}
