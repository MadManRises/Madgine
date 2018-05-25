#include "../interfaceslib.h"

#include "connectionmanager.h"

namespace Engine
{
	SINGLETON_IMPL(SignalSlot::ConnectionManager);

	namespace SignalSlot
	{		
		
		ConnectionManager::ConnectionManager() :
			mThread(std::this_thread::get_id())
		{
		}

		void ConnectionManager::queue(std::function<void()>&& f)
		{
			mMutex.lock();
			mQueue.emplace(std::forward<std::function<void()>>(f));
			mMutex.unlock();
		}

		void ConnectionManager::update()
		{
			while (!mQueue.empty() && mMutex.try_lock())
			{
				std::function<void()> f = move(mQueue.front());
				mQueue.pop();
				mMutex.unlock();
				try
				{
					f();
				}
				catch (const std::exception& e)
				{
					LOG_ERROR("Unhandled Exception during Connection-Dispatching!");
					LOG_EXCEPTION(e);
				}
			}
		}

		std::thread::id ConnectionManager::thread() const
		{
			return mThread;
		}

	}
}