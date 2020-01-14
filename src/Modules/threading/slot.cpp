#include "../moduleslib.h"

#include "slot.h"

#include "defaulttaskqueue.h"

namespace Engine {
namespace Threading {
    SlotBase::SlotBase(TaskQueue *queue)
        : mQueue(queue ? queue : DefaultTaskQueue::getSingletonPtr())
    {
        assert(mQueue);
    }

    void SlotBase::disconnectAll()
    {
        mConnections.clear();
    }

    ConnectionStore &SlotBase::connectionStore()
    {
        return mConnections;
    }

    TaskQueue &SlotBase::taskQueue() const
    {
        return *mQueue;
    }

    void SlotBase::queue(TaskHandle &&task, const std::vector<Threading::DataMutex *> &dependencies) const
    {
        mQueue->queue(std::move(task), dependencies);
    }
}
}