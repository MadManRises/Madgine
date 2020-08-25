#include "../moduleslib.h"

#include "defaulttaskqueue.h"

#include "workgroupstorage.h"

namespace Engine {
namespace Threading {

    WorkgroupLocal<DefaultTaskQueue*> sSingleton;

    DefaultTaskQueue::DefaultTaskQueue(const std::string &name)
        : TaskQueue(name)
    {
        assert(!sSingleton);
        sSingleton = this;
    }

    DefaultTaskQueue::~DefaultTaskQueue()
    {
    }

    DefaultTaskQueue &DefaultTaskQueue::getSingleton()
    {
        return *sSingleton;
    }

    DefaultTaskQueue *DefaultTaskQueue::getSingletonPtr()
    {
        return sSingleton;
    }

}
}