#pragma once

#include "../signalslot/taskqueue.h"
#include "../generic/tupleunpacker.h"
#include "../debug/profiler/profiler.h"
#include "threadapi.h"
#include "../signalslot/taskguard.h"

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

			SignalSlot::TaskQueue &taskQueue();

			static void registerWorkgroupLocalVariable(void(*)(WorkGroup &), void(*)(WorkGroup&));

		private:

			template <typename F, typename... Args>
			int threadMain(const std::string &name, F&& main, Args&&... args)
			{
				setCurrentThreadName(mName + "_" + name);
				try {
					SignalSlot::TaskGuard guard([&]() {mTaskQueue.attachToCurrentThread(); }, [&]() {mTaskQueue.detachFromCurrentThread(); });

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
			std::string mName;
			size_t mInstanceCounter = 0;

			SignalSlot::DefaultTaskQueue mTaskQueue;
			std::vector<std::future<int>> mSubThreads;
			std::vector<SignalSlot::TaskHandle> mThreadInitializers;
			Debug::Profiler::Profiler mProfiler;
		};



		template <typename T, size_t ID = 0 >
		struct WorkgroupLocal
		{
			inline static thread_local T *data = nullptr;
			WorkgroupLocal() {
				WorkGroup::registerWorkgroupLocalVariable(
				[](WorkGroup &group) {
					T *item = new T;
					data = item;
					group.addThreadInitializer([item]() {data = item; });
				},
				[](WorkGroup &group) {
					delete data;
					data = nullptr;
				});
			}

			T *operator->()
			{
				return data;
			}

			T &operator*()
			{
				return *data;
			}
			
		};

	}
}