#pragma once

#include "serialize/serializemanager.h"
#include "signalslot/slot.h"
#include "networkstream.h"

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
			Serialize::StreamError connect(const std::string& url, int portNr, int timeout = 0);
			void connect_async(const std::string& url, int portNr, int timeout = 0);

			void close();


			Serialize::StreamError acceptConnection(int timeout);
			int acceptConnections(int limit = -1);



			bool isConnected() const;

			void moveConnection(Serialize::ParticipantId id, NetworkManager* to);

			template <class T>
			void connectConnectionSlot(T& slot)
			{
				mConnectionResult.connect(slot, SignalSlot::queuedConnection);
			}

		protected:
			void removeSlaveStream() override;
			void removeMasterStream(Serialize::BufferedInOutStream* stream) override;

			Serialize::StreamError addMasterStream(NetworkStream&& stream, bool sendState = true);

			void onConnectionEstablished(int timeout);

		private:
			SocketId mSocket;

			bool mIsServer;

			std::map<Serialize::ParticipantId, NetworkStream> mStreams;
			NetworkStream* mSlaveStream;

			//static constexpr UINT sMessageSignature = 1048;

			static int sManagerCount;

			SignalSlot::Signal<Serialize::StreamError> mConnectionResult;
			SignalSlot::Slot<decltype(&NetworkManager::onConnectionEstablished), &NetworkManager::onConnectionEstablished>
			mConnectionEstablished;
		};
	}
}
