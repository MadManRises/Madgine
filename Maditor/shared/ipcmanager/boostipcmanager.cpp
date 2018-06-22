#include "../maditorsharedlib.h"

#include "boostipcmanager.h"

#include "Madgine/serialize/toplevelserializableunit.h"

#include "../sharedmemory.h"

#include "boostipcstream.h"

#include "boostipcserver.h"

#include "boostipcconnection.h"

namespace Maditor {
	namespace Shared {

		//int NetworkManager::sManagerCount = 0;

		BoostIPCManager::BoostIPCManager(SharedMemory *mem, const std::string &name) :
			mServer(nullptr),
			mIsServer(false),
			mSlaveStream(nullptr),
			mMemory(mem),
			SerializeManager(name),
			mConnectionEstablished(this),
			mAborting(false)
		{
		}

		BoostIPCManager::BoostIPCManager(BoostIPCManager &&other) :
			SerializeManager(std::forward<BoostIPCManager>(other)),
			mServer(other.mServer),
			mIsServer(other.mIsServer),
			mSlaveStream(nullptr),
			mMemory(other.mMemory),
			mConnectionEstablished(this),
			mAborting(false)
		{
			assert(!other.mConnectionThread.joinable());
			for (std::pair<const Engine::Serialize::ParticipantId, BoostIPCStream> &stream : other.mStreams)
				if (!stream.second.isClosed())
					other.moveConnection(stream.first, this);
			other.mServer = nullptr;
			other.mIsServer = false;
			if (other.mSlaveStream) {
				mSlaveStream = new BoostIPCStream(std::move(*other.mSlaveStream), *this);
				setSlaveStream(mSlaveStream, false);
				delete other.mSlaveStream;
				other.mSlaveStream = nullptr;
			}
		}

		BoostIPCManager::~BoostIPCManager()
		{
			close();
		}

		bool BoostIPCManager::startServer()
		{
			if (mServer)
				return false;

			mIsServer = true;

			mServer = mMemory->mgr()->construct<BoostIPCServer>("Server")(mMemory->mgr());

			if (!mServer) {
				return false;
			}
						
			return true;
		}

		bool BoostIPCManager::connect(int timeout)
		{
			if (mServer) {
				mConnectionResult.emit(false);
				return false;
			}

			std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::steady_clock::now();
			mServer = mMemory->mgr()->find<BoostIPCServer>("Server").first;
			while (!mServer) {
				if (std::chrono::duration_cast<std::chrono::milliseconds>
					(std::chrono::steady_clock::now() - start).count() > timeout || mAborting) {
					mConnectionResult.emit(false);
					return false;
				}
				mServer = mMemory->mgr()->find<BoostIPCServer>("Server").first;
			}

			SharedConnectionPtr conn = boost::interprocess::make_managed_shared_ptr(
				mMemory->mgr()->construct<BoostIPCConnection>(boost::interprocess::anonymous_instance)(mMemory->prefix(), mMemory->mgr()), mMemory->memory());
			mServer->enqueue(conn, timeout);

			mConnectionEstablished.queue_direct(std::move(conn), timeout);

			return true;
		}

		void BoostIPCManager::connect_async(int timeout)
		{
			if (!mConnectionThread.joinable()) {
				mAborting = false;
				mConnectionThread = std::thread(&BoostIPCManager::connect, this, timeout);
			}
		}

		void BoostIPCManager::close()
		{
			if (mConnectionThread.joinable())
			{
				mAborting = true;
				mConnectionThread.join();
			}
			if (mServer) {
				removeAllStreams();
				if (mIsServer) {
					mMemory->mgr()->destroy<BoostIPCServer>("Server");
					mIsServer = false;
				}
				assert(mStreams.empty() && mSlaveStream == 0);
				mServer = nullptr;
			}
		}


		void BoostIPCManager::acceptConnections()
		{
			if (mServer) {
				if (mIsServer) {
					while (SharedConnectionPtr conn = mServer->poll()) {
						BoostIPCStream &stream = addMasterStream(BoostIPCStream(std::move(conn), false, *this, createStreamId()));
					}
				}
			}
		}

		bool BoostIPCManager::acceptConnection(int timeout) {
			if (mServer) {
				if (mIsServer) {
					if (SharedConnectionPtr conn = mServer->poll(timeout)) {
						BoostIPCStream &stream = addMasterStream(BoostIPCStream(std::move(conn), false, *this, createStreamId()));
						if (!sendAllMessages(&stream, timeout)) {
							removeMasterStream(&stream);
							return false;
						}
						else {
							return true;
						}
					}
				}
			}
			return false;
		}

		int BoostIPCManager::clientCount()
		{
			return mStreams.size();
		}

		bool BoostIPCManager::isConnected()
		{
			return mServer != nullptr;
		}

		void BoostIPCManager::moveConnection(Engine::Serialize::ParticipantId id, BoostIPCManager * to)
		{
			auto it = mStreams.find(id);
			BoostIPCStream &stream = to->addMasterStream(std::forward<BoostIPCStream>(it->second), false);
			std::vector<Engine::Serialize::TopLevelSerializableUnitBase*> newTopLevels;
			std::set_difference(to->getTopLevelUnits().begin(), to->getTopLevelUnits().end(), getTopLevelUnits().begin(), getTopLevelUnits().end(), std::back_inserter(newTopLevels));
			for (Engine::Serialize::TopLevelSerializableUnitBase *newTopLevel : newTopLevels) {
				sendState(stream, newTopLevel);
			}
		}

		void BoostIPCManager::removeSlaveStream()
		{
			if (mSlaveStream) {
				SerializeManager::removeSlaveStream();
				delete mSlaveStream;
				mSlaveStream = nullptr;
				mServer = nullptr;
			}
		}

		void BoostIPCManager::removeMasterStream(Engine::Serialize::BufferedInOutStream * stream)
		{
			SerializeManager::removeMasterStream(stream);
			assert(mStreams.erase(stream->id()) == 1);
		}

		BoostIPCStream &BoostIPCManager::addMasterStream(BoostIPCStream && stream, bool sendState)
		{
			auto res = mStreams.try_emplace(stream.id(), std::forward<BoostIPCStream>(stream), *this);
			if (!res.second)
				throw 0;
			SerializeManager::addMasterStream(&res.first->second, sendState);
			return res.first->second;
		}

		void BoostIPCManager::onConnectionEstablished(SharedConnectionPtr &&conn, int timeout)
		{
			bool result = true;

			mSlaveStream = new BoostIPCStream(std::forward<SharedConnectionPtr>(conn), true, *this);
			if (setSlaveStream(mSlaveStream, true, timeout) != Engine::Serialize::NO_ERROR) {
				delete mSlaveStream;
				mSlaveStream = 0;
				mServer = nullptr;
				result = false;
			}

			mConnectionResult.emit(result);

		}

	}
}
