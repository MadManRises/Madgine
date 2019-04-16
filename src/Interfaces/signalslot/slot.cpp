#include "../interfaceslib.h"

#include "slot.h"

namespace Engine
{
	namespace SignalSlot
	{
		SlotBase::SlotBase(TaskQueue * queue) :
			mQueue(queue ? queue : DefaultTaskQueue::getSingletonPtr())
		{
			assert(mQueue);
		}

		void SlotBase::disconnectAll()
		{
			mConnections.clear();
		}

		ConnectionStore & SlotBase::connectionStore()
		{
			return mConnections;
		}

		TaskQueue & SlotBase::taskQueue() const
		{
			return *mQueue;
		}
	}
}