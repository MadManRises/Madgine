#pragma once

#include "../signalslot/taskqueue.h"
#include "../generic/tupleunpacker.h"

namespace Engine 
{
	namespace Threading 
	{

		struct INTERFACES_EXPORT WorkGroup 
		{

			template <typename F, typename... Args>
			void createThread(F &&main, Args&&... args)
			{
				std::promise<void> p;
				std::future<void> f = p.get_future();

				mSubThreads.emplace_back(
					std::thread{ &WorkGroup::threadMain<F, std::remove_reference_t<Args>...>, this, std::move(p), std::forward<F>(main), std::forward<Args>(args)... },					
					std::move(f)
				);				
			}

			void addThreadInitializer(SignalSlot::TaskHandle &&task);

			bool singleThreaded();

		private:
			template <typename F, typename... Args>
			void threadMain(std::promise<void> p, F&& main, Args&&... args)
			{
				mTaskQueue.attachToCurrentThread();
				try {
					for (const SignalSlot::TaskHandle &task : mThreadInitializers)
					{
						task();
					}
					TupleUnpacker::invoke(std::forward<F>(main), std::forward<Args>(args)..., *this);
				}
				catch (const std::exception &e) {
					LOG_ERROR(Database::message("Uncaught Exception in Workgroup: ", "")(e.what()));
				}
				catch (...)
				{
					LOG_ERROR("Uncaught unknown Exception in Workgroup!");
				}
				p.set_value();
			}

		private:			
			SignalSlot::DefaultTaskQueue mTaskQueue;
			std::vector<std::pair<std::thread, std::future<void>>> mSubThreads;
			std::vector<SignalSlot::TaskHandle> mThreadInitializers;
		};

	}
}