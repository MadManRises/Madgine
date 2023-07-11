#pragma once

namespace Engine {

template <typename Timepoint = std::chrono::steady_clock::time_point>
struct IntervalClock {

    IntervalClock(Timepoint start)
        : mLastTick(start)
    {
    }

    std::chrono::microseconds tick(Timepoint now)
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(now - std::exchange(mLastTick, now));
    }

private:
    Timepoint mLastTick;
};

}