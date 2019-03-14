#include "../interfaceslib.h"

#include "taskqueue.h"

namespace Engine
{
	namespace SignalSlot
	{

		static thread_local DefaultTaskQueue *sSingleton = nullptr;

		void TaskQueue::execute()
		{
			while (std::optional<TaskHandle> f = fetch())
			{
				(*f)();
			}
		}
		
		DefaultTaskQueue::DefaultTaskQueue() :
			mProxy(nullptr)
		{
			attachToCurrentThread();
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

		void DefaultTaskQueue::attachToCurrentThread()
		{
			assert(!sSingleton);
			sSingleton = this;
		}

		DefaultTaskQueue &DefaultTaskQueue::getSingleton()
		{
			assert(sSingleton);
			return *sSingleton;
		}

		DefaultTaskQueue *DefaultTaskQueue::getSingletonPtr()
		{
			return sSingleton;
		}

	}
}