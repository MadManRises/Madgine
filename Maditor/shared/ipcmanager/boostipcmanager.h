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
			bool connect(int timeout = 0);
			void connect_async(int timeout = 0);

			void close();


			bool acceptConnection(int timeout);
			void acceptConnections();

			int clientCount();

			bool isConnected();

			void moveConnection(Engine::Serialize::ParticipantId id, BoostIPCManager *to);

			template <class T>
			void connectConnectionSlot(T &slot) {
				mConnectionResult.connect(slot, Engine::SignalSlot::queuedConnection);
			}

			using SerializeManager::getSlaveStream;

		protected:
			virtual void removeSlaveStream() override;
			virtual void removeMasterStream(Engine::Serialize::BufferedInOutStream *stream) override;

			BoostIPCStream &addMasterStream(BoostIPCStream &&stream, bool sendState = true);

			void onConnectionEstablished(SharedConnectionPtr &&conn, int timeout);

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
			Engine::SignalSlot::Slot<decltype(&BoostIPCManager::onConnectionEstablished), &BoostIPCManager::onConnectionEstablished> mConnectionEstablished;

		};

	}
}

