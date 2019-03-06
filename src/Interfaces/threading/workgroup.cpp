#include "../interfaceslib.h"

#include "workgroup.h"

namespace Engine
{
	namespace Threading
	{
		void WorkGroup::addThreadInitializer(SignalSlot::TaskHandle && task)
		{
			assert(mSubThreads.empty());
			mThreadInitializers.emplace_back(std::move(task));
		}

		bool WorkGroup::singleThreaded()
		{
			auto pivot =
				std::remove_if(mSubThreads.begin(), mSubThreads.end(), 
					[](std::pair<std::thread, std::future<void>> &p) {
						return p.second.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready; 
					}
			);
			for (auto it = pivot; it != mSubThreads.end(); ++it)
			{
				it->first.join();
			}
			mSubThreads.erase(pivot, mSubThreads.end());
			return mSubThreads.empty();
		}

	}
}
