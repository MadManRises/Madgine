#pragma once

namespace Engine {
namespace Threading {

    struct MODULES_EXPORT AwaitableTimePoint {
        AwaitableTimePoint(std::chrono::steady_clock::duration dur);
        AwaitableTimePoint(std::chrono::steady_clock::time_point point);

        bool await_ready();

        void await_suspend(TaskHandle task);

        void await_resume();

    private:
        std::chrono::steady_clock::time_point mTimePoint;
    };

}
}

MODULES_EXPORT Engine::Threading::AwaitableTimePoint operator co_await(std::chrono::steady_clock::duration dur);
MODULES_EXPORT Engine::Threading::AwaitableTimePoint operator co_await(std::chrono::steady_clock::time_point point);
