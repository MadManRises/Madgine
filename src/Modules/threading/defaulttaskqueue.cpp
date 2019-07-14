#include "../moduleslib.h"

#include "defaulttaskqueue.h"

#include "workgroup.h"

namespace Engine {
namespace Threading {

			DefaultTaskQueue::DefaultTaskQueue()
        : TaskQueue("Default")
    {
    }

    DefaultTaskQueue::~DefaultTaskQueue()
    {
    }

    DefaultTaskQueue &DefaultTaskQueue::getSingleton()
    {
        return Threading::WorkGroup::self().taskQueue();
    }

    DefaultTaskQueue *DefaultTaskQueue::getSingletonPtr()
    {
        return &getSingleton();
    }

}
}