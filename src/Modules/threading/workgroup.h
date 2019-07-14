#pragma once

#include "defaulttaskqueue.h"
#include "../generic/tupleunpacker.h"
#include "Interfaces/threading/threadapi.h"
#include "../signalslot/taskguard.h"

#include "threadlocal.h"

namespace Engine 
{
	namespace Threading 
	{


		struct MODULES_EXPORT WorkGroup 
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
			static void addStaticThreadInitializer(SignalSlot::TaskHandle &&task);

			bool singleThreaded();
			void checkThreadStates();

			const std::string &name() const;

			Threading::DefaultTaskQueue &taskQueue();

			static int registerLocalBssVariable(std::function<Any()> ctor);
                        static int registerLocalObjectVariable(std::function<Any()> ctor);

			static const Any &localVariable(int index);

			static WorkGroup &self();
                        static bool isInitialized();

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

			Threading::DefaultTaskQueue mTaskQueue;
			std::vector<std::future<int>> mSubThreads;
			std::vector<SignalSlot::TaskHandle> mThreadInitializers;

			std::vector<Any> mBssVariables;
                        std::vector<Any> mObjectVariables;
		};



		template <typename T>
		using WorkgroupLocal = ThreadLocal<T, WorkGroup>;


	}
}