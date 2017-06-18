
#include "interfaceslib.h"
#include "connectionstore.h"
#include "connection.h"

namespace Engine {
	namespace SignalSlot {

		std::shared_ptr<ConnectionBase> ConnectionStore::make_shared_connection(ConnectionBase * conn) {
			return std::shared_ptr<ConnectionBase>(conn);
		}

		void ConnectionStore::disconnect(const const_iterator & where)
		{
			mConnections.erase(where);
		}

		void ConnectionStore::clear()
		{
			mConnections.clear();
		}

		ConnectionStore & ConnectionStore::globalStore()
		{
			static ConnectionStore store;
			return store;
		}

		ConnectionManager::ConnectionManager() :
			mThread(std::this_thread::get_id())
		{
		}

		void ConnectionManager::queue(const std::function<void()>& f)
		{
			mMutex.lock();
			mQueue.emplace(f);
			mMutex.unlock();
		}

		void ConnectionManager::update()
		{
			while (!mQueue.empty() && mMutex.try_lock()) {
				std::function<void()> f = std::move(mQueue.front());
				mQueue.pop();
				mMutex.unlock();
				try {
					f();
				}
				catch (const std::exception &e) {
					LOG_ERROR("Unhandled Exception during Connection-Dispatching!");
					LOG_EXCEPTION(e);
				}
			}
		}

		std::thread::id ConnectionManager::thread()
		{
			return mThread;
		}

	}
}
