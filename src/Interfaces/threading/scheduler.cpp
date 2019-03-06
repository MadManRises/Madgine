#include "../interfaceslib.h"

#include "scheduler.h"

#include "frameloop.h"

#include "workgroup.h"

namespace Engine 
{
	namespace Threading
	{
		
		Scheduler::Scheduler(WorkGroup &group, std::vector<SignalSlot::TaskQueue*> additionalQueues) :
			mWorkgroup(group),
			mAdditionalQueues(std::move(additionalQueues))
		{}

		int Scheduler::go()
		{
			for (SignalSlot::TaskQueue *queue : mAdditionalQueues)
			{
				mWorkgroup.createThread(&Scheduler::schedulerLoop, this, queue);
			}

			SignalSlot::TaskQueue *queue = SignalSlot::DefaultTaskQueue::getSingletonPtr();
			while (!queue->empty() || !mWorkgroup.singleThreaded())
			{
				schedulerLoop(queue);
			}

			for (SignalSlot::TaskQueue *queue : mAdditionalQueues)
			{
				assert(queue->empty());
			}
			assert(queue->empty());

			return 0;
		}

		void Scheduler::schedulerLoop(SignalSlot::TaskQueue *queue)
		{
			queue->execute();
		}

	}
}