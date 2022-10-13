#pragma once

namespace Engine {

template <typename Clock>
struct IntervalClock {

    template <typename Duration>
    Duration tick()
    {
        typename Clock::time_point now = Clock::now();
        return std::chrono::duration_cast<Duration>(now - std::exchange(mLastTick, now));
    }

private:
    typename Clock::time_point mLastTick = Clock::now();
};

}