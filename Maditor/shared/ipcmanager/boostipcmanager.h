#pragma once

#include "Madgine/serialize/serializemanager.h"
#include "boostipcstream.h"
#include "Madgine/signalslot/slot.h"


namespace Maditor {
	namespace Shared {

		class MADITOR_SHARED_EXPORT BoostIPCManager : public Engine::Serialize::SerializeManager{

		public:
			BoostIPCManager(SharedMemory *memory, const std::string &name);
			BoostIPCManager(const BoostIPCManager &) = delete;
			BoostIPCManager(BoostIPCManager &&);
			virtual ~BoostIPCManager();


			void operator=(const BoostIPCManager &) = delete;

			bool startServer();
			bool connect(std::chrono::milliseconds timeout = {});
			void connect_async(std::chrono::milliseconds timeout = {});

			void close();


			bool acceptConnection(std::chrono::milliseconds timeout);
			void acceptConnections();

			size_t clientCount();

			bool isConnected();

			void moveConnection(Engine::Serialize::ParticipantId id, BoostIPCManager *to);

			using SerializeManager::getSlaveStream;

			Engine::SignalSlot::SignalStub<bool> &connectionResult();

		protected:
			virtual void removeSlaveStream() override;
			virtual void removeMasterStream(Engine::Serialize::BufferedInOutStream *stream) override;

			BoostIPCStream &addMasterStream(BoostIPCStream &&stream, bool sendState = true);

			void onConnectionEstablished(SharedConnectionPtr &&conn, std::chrono::milliseconds timeout);

		private:
			BoostIPCServer *mServer;
			SharedMemory *mMemory;

			bool mIsServer;			
			bool mAborting;
			std::thread mConnectionThread;

			std::map<Engine::Serialize::ParticipantId, BoostIPCStream> mStreams;
			BoostIPCStream *mSlaveStream;

			//static constexpr UINT sMessageSignature = 1048;

			//static int sManagerCount;

			Engine::SignalSlot::Signal<bool> mConnectionResult;
			Engine::SignalSlot::Slot<&BoostIPCManager::onConnectionEstablished> mConnectionEstablished;

		};

	}
}

