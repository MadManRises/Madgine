#pragma once

namespace Engine
{
	namespace Threading
	{

		struct INTERFACES_EXPORT Scheduler
		{
			Scheduler(WorkGroup &group, std::vector<SignalSlot::TaskQueue*> additionalQueues);

			int go();

		private:
			void schedulerLoop(SignalSlot::TaskQueue *queue);

		private:
			bool mActive = true;
			WorkGroup &mWorkgroup;
			std::vector<SignalSlot::TaskQueue*> mAdditionalQueues;
		};

	}
}