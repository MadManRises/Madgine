#pragma once

#include "../serialize/serializemanager.h"
#include "../signalslot/slot.h"
#include "networkbuffer.h"

namespace Engine
{
	namespace Network
	{
		class INTERFACES_EXPORT NetworkManager : public Serialize::SerializeManager
		{
		public:
			NetworkManager(const std::string& name);
			NetworkManager(const NetworkManager&) = delete;
			NetworkManager(NetworkManager&&) noexcept;
			virtual ~NetworkManager();


			void operator=(const NetworkManager&) = delete;

			bool startServer(int port);
			Serialize::StreamError connect(const std::string& url, int portNr, TimeOut timeout = {});
			void connect_async(const std::string& url, int portNr, TimeOut timeout = {});

			void close();


			Serialize::StreamError acceptConnection(TimeOut timeout);
			int acceptConnections(int limit = -1);



			bool isConnected() const;

			//void moveConnection(Serialize::ParticipantId id, NetworkManager* to);

			SignalSlot::SignalStub<Serialize::StreamError> &connectionResult();

		protected:
			//void removeSlaveStream() override;
			//void removeMasterStream(Serialize::BufferedInOutStream* stream) override;

			//Serialize::StreamError addMasterStream(NetworkStream&& stream, bool sendState = true);

			void onConnectionEstablished(TimeOut timeout);

		private:
			SocketId mSocket;

			bool mIsServer;

			//std::map<Serialize::ParticipantId, NetworkStream> mStreams;
			//std::unique_ptr<NetworkStream> mSlaveStream;

			//static constexpr UINT sMessageSignature = 1048;

			static int sManagerCount;

			SignalSlot::Signal<Serialize::StreamError> mConnectionResult;
			SignalSlot::Slot<&NetworkManager::onConnectionEstablished>
			mConnectionEstablished;
		};
	}
}
