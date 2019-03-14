#pragma once

#include "../signalslot/taskqueue.h"
#include "../generic/tupleunpacker.h"
#include "../debug/profiler/profiler.h"

namespace Engine 
{
	namespace Threading 
	{

		struct INTERFACES_EXPORT WorkGroup 
		{
			WorkGroup();
			~WorkGroup();

			template <typename F, typename... Args>
			void createThread(F &&main, Args&&... args)
			{
				mSubThreads.emplace_back(
					std::async(std::launch::async, &WorkGroup::threadMain<F, std::decay_t<Args>...>, this, std::forward<F>(main), std::forward<Args>(args)...)
				);				
			}

			void addThreadInitializer(SignalSlot::TaskHandle &&task);

			bool singleThreaded();
			void checkThreadStates();

		private:
			template <typename F, typename... Args>
			int threadMain(F&& main, Args&&... args)
			{
				try {
					mTaskQueue.attachToCurrentThread();
					for (const SignalSlot::TaskHandle &task : mThreadInitializers)
					{
						task();
					}
					return TupleUnpacker::invokeDefaultResult(0, std::forward<F>(main), std::forward<Args>(args)..., *this);
				}
				catch (std::exception &e)
				{
					LOG_ERROR("Uncaught Exception in Workgroup-Thread!");
					LOG_EXCEPTION(e);
					throw;
				}
			}

		private:			
			SignalSlot::DefaultTaskQueue mTaskQueue;
			std::vector<std::future<int>> mSubThreads;
			std::vector<SignalSlot::TaskHandle> mThreadInitializers;
			Debug::Profiler::Profiler mProfiler;
		};

	}
}