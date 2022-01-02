#include "../moduleslib.h"

#include "scheduler.h"

#include "workgroup.h"

#include "taskqueue.h"

#include "Interfaces/threading/threadapi.h"

#if EMSCRIPTEN

#    include <emscripten.h>

void emscriptenLoop(void *scheduler)
{
    static_cast<Engine::Threading::Scheduler *>(scheduler)->singleLoop();
}

#endif

namespace Engine {
namespace Threading {

    Scheduler::Scheduler()
        : mWorkgroup(WorkGroup::self())
    {
    }

    int Scheduler::go()
    {
#if EMSCRIPTEN
        emscripten_set_main_loop_arg(&emscriptenLoop, this, 0, false);
#else

        Threading::TaskQueue *main_queue = nullptr;

        for (Threading::TaskQueue *queue : mWorkgroup.taskQueues()) {
            if (queue->wantsMainThread()) {
                if (main_queue)
                    throw 0;
                main_queue = queue;
            }
        }

        if (!main_queue)
            main_queue = mWorkgroup.taskQueues().front();

        for (Threading::TaskQueue *queue : mWorkgroup.taskQueues()) {
            if (queue != main_queue)
                mWorkgroup.createThread(&Scheduler::schedulerLoop, this, queue);
        }

        setCurrentThreadName(mWorkgroup.name() + "_" + main_queue->name() + " (Main)");

        do {
            std::chrono::steady_clock::time_point nextAvailableTaskTime = main_queue->update();
            main_queue->waitForTasks(nextAvailableTaskTime);
            mWorkgroup.checkThreadStates();
            if (!main_queue->running() && main_queue->idle()) {
                for (Threading::TaskQueue *queue : mWorkgroup.taskQueues()) {
                    queue->stop();
                }
            }
        } while (!main_queue->idle() || !mWorkgroup.singleThreaded());

        for (Threading::TaskQueue *queue : mWorkgroup.taskQueues()) {
            assert(queue->idle());
        }

#endif
        return 0;
    }

#if !EMSCRIPTEN
    void Scheduler::schedulerLoop(Threading::TaskQueue *queue)
    {
        setCurrentThreadName(mWorkgroup.name() + "_" + queue->name());
        while (queue->running() || !queue->idle()) {
            std::chrono::steady_clock::time_point nextAvailableTaskTime = queue->update();
            queue->waitForTasks(nextAvailableTaskTime);
        }        
    }
#endif

    void Scheduler::singleLoop()
    {
        for (Threading::TaskQueue *queue : mWorkgroup.taskQueues()) {
            queue->update(1);
        }
    }

}
}