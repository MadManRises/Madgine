#include "../interfaceslib.h"

#include "taskqueue.h"

namespace Engine
{
	SINGLETON_IMPL(SignalSlot::DefaultTaskQueue);

	namespace SignalSlot
	{		

		void TaskQueue::execute()
		{
			while (std::optional<TaskHandle> f = fetch())
			{
				try
				{
					(*f)();
				}
				catch (const std::exception& e)
				{
					LOG_ERROR("Unhandled Exception during Task-Dispatching!");
					LOG_ERROR(e.what());
				}
			}
		}
		
		DefaultTaskQueue::DefaultTaskQueue() :
			mProxy(nullptr)
		{
		}

		void DefaultTaskQueue::queue(TaskHandle&& f)
		{
			if (mProxy)
			{
				mProxy->queue(std::move(f));
			}
			else 
			{
				std::lock_guard<std::mutex> lock(mMutex);
				mQueue.emplace_back(std::move(f));
			}
		}

		std::optional<TaskHandle> DefaultTaskQueue::fetch()
		{
			if (mProxy)
			{
				return mProxy->fetch();
			}
			else
			{
				std::lock_guard<std::mutex> lock(mMutex);
				if (!mQueue.empty())
				{
					TaskHandle f = std::move(mQueue.front());
					mQueue.pop_front();					
					return f;
				}
				else
				{					
					return {};
				}
			}
		}

		bool DefaultTaskQueue::empty()
		{
			std::lock_guard<std::mutex> lock(mMutex);
			return mQueue.empty();
		}

	}
}