#include "../interfaceslib.h"

#include "workgroup.h"

namespace Engine
{
	namespace Threading
	{
		WorkGroup::WorkGroup() :
			mProfiler(*this)
		{}

		WorkGroup::~WorkGroup()
		{
			assert(singleThreaded());
		}

		void WorkGroup::addThreadInitializer(SignalSlot::TaskHandle && task)
		{
			assert(mSubThreads.empty());
			mThreadInitializers.emplace_back(std::move(task));
		}

		bool WorkGroup::singleThreaded()
		{
			return mSubThreads.empty();
		}

		void WorkGroup::checkThreadStates()
		{
			auto pivot =
				std::remove_if(mSubThreads.begin(), mSubThreads.end(),
					[](std::future<int> &f)
				{
					return f.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
				}
			);

			for (auto it = pivot; it != mSubThreads.end(); ++it)
			{
				it->get();
			}

			mSubThreads.erase(pivot, mSubThreads.end());
		}

	}
}
