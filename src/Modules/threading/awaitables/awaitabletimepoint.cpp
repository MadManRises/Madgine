#include "../../moduleslib.h"

#include "awaitabletimepoint.h"

#include "../taskhandle.h"
#include "../taskqueue.h"

namespace Engine {
namespace Threading {

    AwaitableTimePoint::AwaitableTimePoint(std::chrono::steady_clock::duration dur)
        : mTimePoint(std::chrono::steady_clock::now() + dur)
    {
    }

    AwaitableTimePoint::AwaitableTimePoint(std::chrono::steady_clock::time_point point)
        : mTimePoint(point)
    {
    }

    bool AwaitableTimePoint::await_ready()
    {
        return false;
    }

    void AwaitableTimePoint::await_suspend(TaskHandle task)
    {
        task.queue()->queueHandle_for(std::move(task), mTimePoint);
    }

    void AwaitableTimePoint::await_resume() {

    }
}
}

Engine::Threading::AwaitableTimePoint operator co_await(std::chrono::steady_clock::duration dur)
{
    return dur;
}

Engine::Threading::AwaitableTimePoint operator co_await(std::chrono::steady_clock::time_point point)
{
    return point;
}