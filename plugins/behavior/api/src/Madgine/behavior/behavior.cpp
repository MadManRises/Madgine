#include "../behaviorlib.h"

#include "behavior.h"

namespace Engine {
namespace Behavior {

    void Behavior::destroyState(BehaviorStateBase *state)
    {
        state->destroy();
    }

    Behavior::Behavior(StatePtr state)
        : mState(std::move(state))
    {
    }

    Behavior &Behavior::operator=(StatePtr state)
    {
        mState = std::move(state);
        return *this;
    }

    Behavior::state::state(StatePtr state)
        : mState(std::move(state))
    {
        mState->mReceiver.set(*this);
    }

    void Behavior::state::start()
    {
        mState->start();
    }

    void Behavior::state::stop()
    {
        mState->stop();
    }

    Behavior::StatePtr Behavior::connect(BehaviorReceiver &receiver)
    {
        mState->mReceiver.set(receiver);
        return std::move(mState);
    }

}
}