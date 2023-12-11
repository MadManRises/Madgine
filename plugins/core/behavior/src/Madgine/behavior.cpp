#include "behaviorlib.h"

#include "behavior.h"

#include "Meta/keyvalue/valuetype.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

METATABLE_BEGIN(Engine::BehaviorStateBase)
METATABLE_END(Engine::BehaviorStateBase)

namespace Engine {

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

Behavior CoroutineBehaviorState::get_return_object()
{
    return Behavior::StatePtr { this };
}

void CoroutineBehaviorState::start(Behavior::receiver *rec, ArgumentList arguments, BehaviorTrackerContext context, std::stop_token stopToken)
{
    mReceiver = rec;
    mArguments = std::move(arguments);
    mContext = context;
    mStopToken = std::move(stopToken);
    std::coroutine_handle<CoroutineBehaviorState>::from_promise(*this).resume();
}

void CoroutineBehaviorState::destroy()
{
    std::coroutine_handle<CoroutineBehaviorState>::from_promise(*this).destroy();
}

void CoroutineBehaviorState::visit(CallableView<void(const Execution::StateDescriptor &)> visitor)
{
}

CoroutineBehaviorState::InitialSuspend CoroutineBehaviorState::initial_suspend() noexcept
{
    return {};
}

bool CoroutineBehaviorState::FinalSuspend::await_ready() noexcept
{
    return false;
}

void CoroutineBehaviorState::FinalSuspend::await_suspend(std::coroutine_handle<CoroutineBehaviorState> handle) noexcept
{
    handle.promise().mReceiver->set_value({});
}

void CoroutineBehaviorState::FinalSuspend::await_resume() noexcept
{
    throw 0;
}

CoroutineBehaviorState::FinalSuspend CoroutineBehaviorState::final_suspend() noexcept
{
    return {};
}

void CoroutineBehaviorState::return_void()
{
    //mValue = void
}

void CoroutineBehaviorState::unhandled_exception()
{
    throw 0;
}

void CoroutineBehaviorState::set_error(InterpretResult result)
{
    mReceiver->set_error(result);
}

void CoroutineBehaviorState::set_done()
{
    mReceiver->set_done();
}

std::string CoroutineBehaviorState::name() const
{
    return mStacktrace.calculateReadable().front().mFunction;
}

bool CoroutineBehaviorState::InitialSuspend::await_ready() noexcept
{
    return false;
}

void CoroutineBehaviorState::InitialSuspend::await_suspend(std::coroutine_handle<CoroutineBehaviorState> handle) noexcept
{
    handle.promise().mStacktrace = Debug::StackTrace<1>::getCurrent(1);
}

void CoroutineBehaviorState::InitialSuspend::await_resume() noexcept
{
}

}