#pragma once

#include "workgroup.h"
#include "../generic/tupleunpacker.h"

namespace Engine
{
	namespace Threading
	{
	
		struct WorkGroupHandle
		{
			template <typename F, typename... Args>
			WorkGroupHandle(F&& main, Args&&... args) :
				mThread(&WorkGroupHandle::threadMain<F, Args...>, std::forward<F>(main), std::forward<Args>(args)...)
			{				
			}

			~WorkGroupHandle() 
			{
				if (mThread.joinable())
				{					
					mThread.join();
				}
			}

		private:
			template <typename F, typename... Args>
			static void threadMain(F&& main, Args&&... args)
			{
				WorkGroup group;
				try {
					TupleUnpacker::invoke(std::forward<F>(main), std::forward<Args>(args)..., group);
				}
				catch (const std::exception &e) {
					LOG_ERROR(Database::message("Uncaught Exception in Workgroup: ", "")(e.what()));
				}
				catch (...)
				{
					LOG_ERROR("Uncaught unknown Exception in Workgroup!");
				}
			}

		private:
			std::thread mThread;			
		};

	}
}