#include "../moduleslib.h"

#include "defaulttaskqueue.h"

#include "workgroupstorage.h"

namespace Engine {
namespace Threading {

    WorkgroupLocal<DefaultTaskQueue> sSingleton;

    DefaultTaskQueue::DefaultTaskQueue()
        : TaskQueue("Default")
    {
    }

    DefaultTaskQueue::~DefaultTaskQueue()
    {
    }

    DefaultTaskQueue &DefaultTaskQueue::getSingleton()
    {
        return sSingleton;
    }

    DefaultTaskQueue *DefaultTaskQueue::getSingletonPtr()
    {
        return &getSingleton();
    }

}
}