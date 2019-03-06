#pragma once

#include "../generic/singleton.h"
#include "task.h"

namespace Engine
{
	namespace SignalSlot
	{

		struct INTERFACES_EXPORT TaskQueue
		{
			virtual void queue(TaskHandle&& task) = 0;

			virtual std::optional<TaskHandle> fetch() = 0;

			virtual bool empty() = 0;

			void execute();
		};
		
		struct INTERFACES_EXPORT DefaultTaskQueue : Singleton<DefaultTaskQueue>,
			TaskQueue
		{
		public:
			DefaultTaskQueue();
			DefaultTaskQueue(const DefaultTaskQueue&) = delete;

			DefaultTaskQueue &operator=(const DefaultTaskQueue&) = delete;

			virtual void queue(TaskHandle &&task) override;

			virtual std::optional<TaskHandle> fetch() override;

			virtual bool empty() override;

			using Singleton::attachToCurrentThread;

		private:

			TaskQueue *mProxy;
			std::list<TaskHandle> mQueue;
			std::mutex mMutex;
		};

	}
}