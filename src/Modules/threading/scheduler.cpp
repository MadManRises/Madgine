#include "../moduleslib.h"

#include "scheduler.h"

#include "workgroup.h"

#include "defaulttaskqueue.h"

#if EMSCRIPTEN

#include <emscripten.h>

void emscriptenLoop(void *scheduler)
{
    static_cast<Engine::Threading::Scheduler *>(scheduler)->singleLoop();
}

#endif

namespace Engine {
namespace Threading {

    Scheduler::Scheduler(WorkGroup &group, std::vector<Threading::TaskQueue *> additionalQueues)
        : mWorkgroup(group)
        , mAdditionalQueues(std::move(additionalQueues))
    {
    }

    int Scheduler::go()
    {
#if EMSCRIPTEN
        emscripten_set_main_loop_arg(&emscriptenLoop, this, 0, false);
#else
        for (Threading::TaskQueue *queue : mAdditionalQueues) {
            mWorkgroup.createNamedThread(queue->name(), &Scheduler::schedulerLoop, this, queue);
        }

        Threading::TaskQueue *queue = DefaultTaskQueue::getSingletonPtr();

        do {
            std::chrono::steady_clock::time_point nextAvailableTaskTime;
            queue->update(nextAvailableTaskTime);
            nextAvailableTaskTime = std::min(std::chrono::steady_clock::now() + std::chrono::milliseconds(200), nextAvailableTaskTime);
            queue->waitForTasks(nextAvailableTaskTime);
            mWorkgroup.checkThreadStates();
        } while (!queue->idle() || !mWorkgroup.singleThreaded());

        for (Threading::TaskQueue *queue : mAdditionalQueues) {
            assert(queue->idle());
        }
        assert(queue->idle());
#endif
        return 0;
    }

    void Scheduler::schedulerLoop(Threading::TaskQueue *queue)
    {
        while (!queue->idle() || queue->running()) {
            std::chrono::steady_clock::time_point nextAvailableTaskTime = std::chrono::steady_clock::time_point::max();
            queue->update(nextAvailableTaskTime);
            queue->waitForTasks(nextAvailableTaskTime);
        }
    }

    void Scheduler::singleLoop()
    {
        DefaultTaskQueue::getSingleton().update(1);
        for (Threading::TaskQueue *queue : mAdditionalQueues) {
            queue->update(1);
        }
    }

}
}