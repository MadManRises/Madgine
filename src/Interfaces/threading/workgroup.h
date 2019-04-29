#pragma once

#include "../signalslot/taskqueue.h"
#include "../generic/tupleunpacker.h"
#include "../debug/profiler/profiler.h"
#include "threadapi.h"
#include "../signalslot/taskguard.h"

#include "threadlocal.h"

namespace Engine 
{
	namespace Threading 
	{


		struct INTERFACES_EXPORT WorkGroup 
		{
			WorkGroup(const std::string &name = "");
			~WorkGroup();

			template <typename F, typename... Args>
			void createThread(F &&main, Args&&... args)
			{
				createNamedThread("thread_" + std::to_string(mInstanceCounter), std::forward<F>(main), std::forward<Args>(args)...);
			}

			template <typename F, typename... Args>
			void createNamedThread(const std::string &name, F &&main, Args&&... args)
			{
				mSubThreads.emplace_back(
					std::async(std::launch::async, &WorkGroup::threadMain<F, std::decay_t<Args>...>, this, name, std::forward<F>(main), std::forward<Args>(args)...)
				);
			}


			void addThreadInitializer(SignalSlot::TaskHandle &&task);

			bool singleThreaded();
			void checkThreadStates();

			const std::string &name() const;

			SignalSlot::DefaultTaskQueue &taskQueue();

			static size_t registerLocalVariable(Any &&);

			static const Any &localVariable(size_t index);

			static WorkGroup &self();

		private:

			void initThread(const std::string &name);

			template <typename F, typename... Args>
			int threadMain(const std::string &name, F&& main, Args&&... args)
			{
				initThread(name);
				try {
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
			std::string mName;
			size_t mInstanceCounter = 0;

			SignalSlot::DefaultTaskQueue mTaskQueue;
			std::vector<std::future<int>> mSubThreads;
			std::vector<SignalSlot::TaskHandle> mThreadInitializers;
			Debug::Profiler::Profiler mProfiler;

			std::vector<Any> mLocalVariables;
		};



		template <typename T>
		using WorkgroupLocal = ThreadLocal<T, WorkGroup>;


	}
}