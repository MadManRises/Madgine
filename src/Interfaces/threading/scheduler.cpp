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
				mWorkgroup.createNamedThread(queue->name(), &Scheduler::schedulerLoop, this, queue);
			}

			SignalSlot::TaskQueue *queue = SignalSlot::DefaultTaskQueue::getSingletonPtr();
			schedulerLoop(queue);
			while (!mWorkgroup.singleThreaded())
			{
				schedulerLoop(queue);
				mWorkgroup.checkThreadStates();
			}

			for (SignalSlot::TaskQueue *queue : mAdditionalQueues)
			{
				assert(queue->idle());
			}
			assert(queue->idle());

			return 0;
		}

		void Scheduler::schedulerLoop(SignalSlot::TaskQueue *queue)
		{			
			while (!queue->idle() || queue->running())
			{
				std::chrono::steady_clock::time_point nextAvailableTaskTime;
				queue->update(nextAvailableTaskTime);
				queue->waitForTasks(nextAvailableTaskTime);				
			}
		}

	}
}