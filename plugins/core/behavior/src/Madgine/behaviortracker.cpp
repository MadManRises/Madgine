#include "behaviorlib.h"

#include "behaviortracker.h"

namespace Engine {

void BehaviorTracker::registerState(BehaviorTrackerState *state)
{
    std::lock_guard guard { mMutex };
    mStates.push_back(state);
}

void BehaviorTracker::unregisterState(BehaviorTrackerState *state)
{
    std::lock_guard guard { mMutex };
    size_t count = std::erase(mStates, state);
    assert(count == 1);
}

BehaviorTracker::AccessGuard BehaviorTracker::guarded() const
{
    return { std::lock_guard { mMutex }, mStates };
}

}